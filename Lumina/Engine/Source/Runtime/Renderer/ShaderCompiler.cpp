#include "ShaderCompiler.h"

#include "Assets/AssetPath.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"
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
        {
            FScopeLock Lock(RunMutex);

            FRequest Request;
            Request.Path = ShaderPath;
            Request.CompileOptions = CompileOptions;
            Request.OnCompleted = Memory::Move(OnCompleted);

            CompileRequests.push(std::move(Request));
        }

        CompileCV.notify_one();
        return true;
    }
    
    void FSpirVShaderCompiler::OnCompileThread()
    {
        Threading::InitializeThreadHeap();
    
        while (bCompileThreadRunning)
        {
            FRequest Request;
    
            // Wait for available request
            {
                std::unique_lock<FMutex> Lock(RunMutex);
                CompileCV.wait(Lock, [&]
                {
                    return !CompileRequests.empty() || !bCompileThreadRunning;
                });
    
                if (!bCompileThreadRunning)
                {
                    break;
                }
    
                Request = std::move(CompileRequests.front());
                CompileRequests.pop();
            }
    
            // === Begin Compilation ===
    
            LOG_DEBUG("Compiling Shader: {0}", Request.Path);
    
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
                continue;
            }
    
            auto Preprocessed = Compiler.PreprocessGlsl(RawShaderString.c_str(),
                                                        RawShaderString.size(),
                                                        shaderc_glsl_infer_from_source,
                                                        Request.Path.c_str(), Options);
    
            if (Preprocessed.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Preprocessing failed: {0} - {1}", Request.Path, Preprocessed.GetErrorMessage());
                continue;
            }
    
            FString PreprocessedShader(Preprocessed.begin(), Preprocessed.end());
    
            auto CompileResult = Compiler.CompileGlslToSpv(PreprocessedShader.c_str(),
                                                           PreprocessedShader.size(),
                                                           shaderc_glsl_infer_from_source,
                                                           Request.Path.c_str(), Options);
    
            if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR("Compilation failed: {0} - {1}", Request.Path, CompileResult.GetErrorMessage());
                continue;
            }
    
            Binaries.assign(CompileResult.begin(), CompileResult.end());
    
            if (Binaries.empty())
            {
                LOG_ERROR("Shader compiled to empty SPIR-V: {0}", Request.Path);
                continue;
            }
    
            Request.OnCompleted(Binaries);
        }
    
        Threading::ShutdownThreadHeap();
    }
    
    void FSpirVShaderCompiler::Initialize()
    {
        CompileThread = std::thread(&FSpirVShaderCompiler::OnCompileThread, this);
    }

    void FSpirVShaderCompiler::Shutdown()
    {
        bCompileThreadRunning = false;
        CompileCV.notify_all();

        if (CompileThread.joinable())
        {
            CompileThread.join();
        }
    }
}
