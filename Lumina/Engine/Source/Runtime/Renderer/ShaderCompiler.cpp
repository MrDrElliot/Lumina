#include "ShaderCompiler.h"

#include "RenderResource.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "shaderc/shaderc.hpp"
#include "TaskSystem/TaskSystem.h"
#include "SPIRV-Reflect/spirv_reflect.h"

namespace Lumina
{
    
    class FShaderCIncluder : public shaderc::CompileOptions::IncluderInterface
    {
        shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override
        {
            std::filesystem::path RequestingPath = requesting_source;
            std::filesystem::path TruePath = RequestingPath.parent_path() / requested_source;
            FString IncludePath = FString(TruePath.string().c_str());
            FString ShaderData;
            
            if (FileHelper::LoadFileIntoString(ShaderData, IncludePath))
            {
                auto* result = new shaderc_include_result;
                result->source_name = requested_source;
                result->source_name_length = strlen(requested_source);
                result->content = new char[ShaderData.size() + 1];
                result->content_length = ShaderData.size();

                memcpy((void*)result->content, ShaderData.data(), ShaderData.size() + 1);
                return result;
            }

            return nullptr;
        }

        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete[] data->content;
            delete data;
        }
    };

    void FSpirVShaderCompiler::ReflectSpirv(TSpan<uint32> SpirV, FShaderReflection& Reflection)
    {
        SpvReflectShaderModule Module;
        SpvReflectResult Result = spvReflectCreateShaderModule(SpirV.size_bytes(), SpirV.data(), &Module);
        if (Result != SPV_REFLECT_RESULT_SUCCESS)
        {
            LOG_ERROR("Failed to create SPIR-V Reflect Module!");
            return;
        }
        
        switch (Module.shader_stage)
        {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            Reflection.ShaderType = ERHIShaderType::Vertex;
            break;
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            Reflection.ShaderType = ERHIShaderType::Fragment;
            break;
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            Reflection.ShaderType = ERHIShaderType::Compute;
            break;
        }

        uint32 NumDescriptorSets = 0;
        Result = spvReflectEnumerateDescriptorSets(&Module, &NumDescriptorSets, nullptr);
        if (Result != SPV_REFLECT_RESULT_SUCCESS)
        {
            LOG_ERROR("Failed to enumerate descriptor sets (count).");
            return;
        }

        TVector<SpvReflectDescriptorSet*> ReflectSets(NumDescriptorSets);
        Result = spvReflectEnumerateDescriptorSets(&Module, &NumDescriptorSets, ReflectSets.data());
        if (Result != SPV_REFLECT_RESULT_SUCCESS)
        {
            LOG_ERROR("Failed to enumerate descriptor sets (data).");
            return;
        }

        
        for (uint32 SetIndex = 0; SetIndex < NumDescriptorSets; ++SetIndex)
        {
            SpvReflectDescriptorSet* ReflectSet = ReflectSets[SetIndex];

            for (uint32 BindingIndex = 0; BindingIndex < ReflectSet->binding_count; ++BindingIndex)
            {
                SpvReflectDescriptorBinding* Binding = ReflectSet->bindings[BindingIndex];

                Reflection.Bindings.push_back();
                FShaderBinding& NewBinding = Reflection.Bindings.back();
                
                NewBinding.Name = Binding->name ? Binding->name : "";
                NewBinding.Set = Binding->set;
                NewBinding.Binding = Binding->binding;

                if (Binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                {
                    NewBinding.Type = ERHIBindingResourceType::Buffer_CBV;
                    NewBinding.Size = Binding->block.size;
                }
                else if (Binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    NewBinding.Type = ERHIBindingResourceType::Buffer_UAV;
                    NewBinding.Size = Binding->block.size;
                }
                else if (Binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                {
                    NewBinding.Type = ERHIBindingResourceType::Texture_SRV;
                }
                else if (Binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER)
                {
                    NewBinding.Type = ERHIBindingResourceType::Sampler;
                }
                else if (Binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                {
                    NewBinding.Type = ERHIBindingResourceType::Texture_UAV;
                }
            }
        }
        
        spvReflectDestroyShaderModule(&Module);
    }
    
    
    bool FSpirVShaderCompiler::CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted)
    {
        FRequest Request;
        Request.Path = ShaderPath;
        Request.CompileOptions = CompileOptions;
        Request.OnCompleted = Memory::Move(OnCompleted);
        PushRequest(Request);
        
        Task::AsyncTask(1, [this, Request = Memory::Move(Request)] (uint32, uint32, uint32)
        {
            FString FileName = Paths::FileName(Request.Path);
            LOG_DEBUG("Compiling Shader: {0} - Thread: {1}", FileName, Threading::GetThreadID());
    
            TVector<uint32> Binaries;

            shaderc::CompileOptions Options;
            Options.SetIncluder(std::make_unique<FShaderCIncluder>());
            Options.SetOptimizationLevel(shaderc_optimization_level_performance);
            Options.SetGenerateDebugInfo();
            Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
            
            for (const FString& Macro : Request.CompileOptions.MacroDefinitions)
            {
                Options.AddMacroDefinition(Macro.c_str());
            }
    
            FString RawShaderString;
            if (!FileHelper::LoadFileIntoString(RawShaderString, Request.Path))
            {
                LOG_ERROR("Failed to load shader: {0}", Request.Path);
                PopRequest();
                return;
            }

            
            auto Preprocessed = Compiler.PreprocessGlsl(RawShaderString.c_str(),
                                                        RawShaderString.size(),
                                                        shaderc_glsl_infer_from_source,
                                                        Request.Path.c_str(), Options);
    
            if (Preprocessed.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Preprocessing failed: {0} - {1}", Request.Path, Preprocessed.GetErrorMessage());
                PopRequest();
                return;
            }
    
            FString PreprocessedShader(Preprocessed.begin(), Preprocessed.end());
    
            auto CompileResult = Compiler.CompileGlslToSpv(PreprocessedShader.c_str(),
                                                           PreprocessedShader.size(),
                                                           shaderc_glsl_infer_from_source,
                                                           Request.Path.c_str(), Options);
            
            if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Compilation failed: {0} - {1}", Request.Path, CompileResult.GetErrorMessage());
                PopRequest();
                return;
            }
    
            Binaries.assign(CompileResult.begin(), CompileResult.end());
    
            if (Binaries.empty())
            {
                LOG_ERROR("Shader compiled to empty SPIR-V: {0}", Request.Path);
                PopRequest();
                return;
            }

            
            FShaderHeader Shader;
            Shader.Binaries = Memory::Move(Binaries);
            
            ReflectSpirv(Shader.Binaries, Shader.Reflection);
            
            Request.OnCompleted(Memory::Move(Shader));
            
            PopRequest();
        });
        
        return true;
    }
    
    void FSpirVShaderCompiler::PushRequest(const FRequest& Request)
    {
        FScopeLock Lock(RequestMutex);

        PendingRequest.push(Request);
    }

    void FSpirVShaderCompiler::PopRequest()
    {
        FScopeLock Lock(RequestMutex);

        PendingRequest.pop();
    }


    FSpirVShaderCompiler::FSpirVShaderCompiler()
    {
    }

    void FSpirVShaderCompiler::Initialize()
    {
        
    }

    void FSpirVShaderCompiler::Shutdown()
    {
        
    }

    bool FSpirVShaderCompiler::CompilerShaderRaw(FStringView ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted)
    {
        FRequest Request;
        Request.Path = ShaderString;
        Request.CompileOptions = CompileOptions;
        Request.OnCompleted = Memory::Move(OnCompleted);
        
        PushRequest(Request);
        
        Task::AsyncTask(1, [this, Request = Memory::Move(Request)] (uint32 Start, uint32 End, uint32 ThreadNum_)
        {
            shaderc::CompileOptions Options;
            Options.SetIncluder(std::make_unique<FShaderCIncluder>());
            Options.SetOptimizationLevel(shaderc_optimization_level_performance);
            Options.SetGenerateDebugInfo();
            Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
            
            FString VertexPath = Paths::GetEngineResourceDirectory() + "/Shaders/Material.frag";
            
            TVector<uint32> Binaries;
            for (const FString& Macro : Request.CompileOptions.MacroDefinitions)
            {
                Options.AddMacroDefinition(Macro.c_str());
            }
             
            auto Preprocessed = Compiler.PreprocessGlsl(Request.Path.c_str(),
                                                        Request.Path.size(),
                                                        shaderc_glsl_infer_from_source,
                                                        VertexPath.c_str(), Options);
    
            if (Preprocessed.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Preprocessing failed: - {}", Preprocessed.GetErrorMessage());
                PopRequest();
                return;
            }
    
            FString PreprocessedShader(Preprocessed.begin(), Preprocessed.end());
    
            auto CompileResult = Compiler.CompileGlslToSpv(PreprocessedShader.c_str(),
                                                           PreprocessedShader.size(),
                                                           shaderc_glsl_infer_from_source,
                                                           VertexPath.c_str(), Options);
    
            if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Compilation failed: - {}", CompileResult.GetErrorMessage());
                PopRequest();
                return;
            }
    
            Binaries.assign(CompileResult.begin(), CompileResult.end());
            
            if (Binaries.empty())
            {
                LOG_ERROR("Shader compiled to empty SPIR-V");
                PopRequest();
                return;
            }

            
            FShaderHeader Shader;
            Shader.Binaries = Memory::Move(Binaries);
            
            ReflectSpirv(Shader.Binaries, Shader.Reflection);
            Request.OnCompleted(Memory::Move(Shader));
            
            PopRequest();
        });
        
        return true;
    }
}
