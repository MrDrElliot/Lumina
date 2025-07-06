#include "ShaderCompiler.h"

#include "Assets/AssetPath.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "shaderc/shaderc.hpp"
#include "TaskSystem/TaskSystem.h"

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
            
            FileHelper::LoadFileIntoString(ShaderData, IncludePath);
            
            auto* result = new shaderc_include_result;
            result->source_name = requested_source;
            result->source_name_length = strlen(requested_source);
            result->content = new char[ShaderData.size() + 1];
            result->content_length = ShaderData.size();

            memcpy((void*)result->content, ShaderData.data(), ShaderData.size() + 1);
            return result;
        }

        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete[] data->content;
            delete data;
        }
    };

    bool FSpirVShaderCompiler::CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted)
    {
        FScopeLock Lock(RunMutex);

        FRequest Request;
        Request.Path = ShaderPath;
        Request.CompileOptions = CompileOptions;
        Request.OnCompleted = Memory::Move(OnCompleted);

        FTaskSystem::Get()->ScheduleLambda([Request]
        {
            FString FileName = Paths::FileName(Request.Path);
            LOG_DEBUG("Compiling Shader: {0} - Thread: {1}", FileName, Threading::GetThreadID());
    
            TVector<uint32> Binaries;
            shaderc::Compiler Compiler;
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
                return;
            }

            
            auto Preprocessed = Compiler.PreprocessGlsl(RawShaderString.c_str(),
                                                        RawShaderString.size(),
                                                        shaderc_glsl_infer_from_source,
                                                        Request.Path.c_str(), Options);
    
            if (Preprocessed.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Preprocessing failed: {0} - {1}", Request.Path, Preprocessed.GetErrorMessage());
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
                return;
            }
    
            Binaries.assign(CompileResult.begin(), CompileResult.end());
    
            if (Binaries.empty())
            {
                LOG_ERROR("Shader compiled to empty SPIR-V: {0}", Request.Path);
                return;
            }
    
            Request.OnCompleted(Binaries);
        });
        
        return true;
    }
    
   
    
    void FSpirVShaderCompiler::Initialize()
    {
        
    }

    void FSpirVShaderCompiler::Shutdown()
    {

    }

    bool FSpirVShaderCompiler::CompilerShaderRaw(const FString& ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted)
    {
        FRequest Request;
        Request.Path = ShaderString;
        Request.CompileOptions = CompileOptions;
        Request.OnCompleted = Memory::Move(OnCompleted);

        FTaskSystem::Get()->ScheduleLambda([Request]
        {
            FString VertexPath = std::filesystem::path(Paths::GetEngineResourceDirectory() / "Shaders/Material.frag").generic_string().c_str();
            
            TVector<uint32> Binaries;
            shaderc::Compiler Compiler;
            shaderc::CompileOptions Options;
            Options.SetIncluder(std::make_unique<FShaderCIncluder>());
            Options.SetOptimizationLevel(shaderc_optimization_level_performance);
            Options.SetGenerateDebugInfo();
            Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
    
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
                return;
            }
    
            Binaries.assign(CompileResult.begin(), CompileResult.end());
    
            if (Binaries.empty())
            {
                LOG_ERROR("Shader compiled to empty SPIR-V");
                return;
            }
    
            Request.OnCompleted(Binaries);
        });
        
        return true;
    }
}
