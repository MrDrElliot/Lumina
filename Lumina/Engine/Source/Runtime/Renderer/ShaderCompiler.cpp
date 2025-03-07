#include "ShaderCompiler.h"

#include "Assets/AssetPath.h"
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
            
            FFileHelper::LoadFileIntoString(ShaderData, IncludePath);
            
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
    
    bool FSpirVShaderCompiler::CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, TVector<uint32>& OutBinaries)
    {
        LOG_DEBUG("Compiling Shader: {0}", ShaderPath);
        
        shaderc::Compiler Compiler;
        shaderc::CompileOptions Options;
        Options.SetIncluder(std::make_unique<FShaderCIncluder>());
        Options.SetOptimizationLevel(shaderc_optimization_level_performance);
        Options.SetGenerateDebugInfo();
        Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        
        for (const FString& Option : CompileOptions.MacroDefinitions)
        {
            Options.AddMacroDefinition(Option.c_str());
        }

        FString RawShaderString;
        if (!FFileHelper::LoadFileIntoString(RawShaderString, ShaderPath))
        {
            return false;
        }

        shaderc::PreprocessedSourceCompilationResult PreprocessedResult = Compiler.PreprocessGlsl(RawShaderString.c_str(),
            RawShaderString.size(),
            shaderc_shader_kind::shaderc_glsl_infer_from_source,
            ShaderPath.c_str(),
            Options);

        if (PreprocessedResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_ERROR("Shader {0} failed compilation during pre-processing: {1}", ShaderPath, PreprocessedResult.GetErrorMessage());
            return false;
        }

        FString PreprocessedShader(PreprocessedResult.begin(), PreprocessedResult.end());
        
        auto CompileResult = Compiler.CompileGlslToSpv(PreprocessedShader.c_str(),
            PreprocessedShader.size(),
            shaderc_shader_kind::shaderc_glsl_infer_from_source, ShaderPath.c_str(), Options);

        
        if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_ERROR("Shader {0} failed compilation during compilation: {1}", ShaderPath, CompileResult.GetErrorMessage());
            return false;
        }

        OutBinaries.assign(CompileResult.begin(), CompileResult.end());
        
        if (OutBinaries.empty())
        {
            LOG_ERROR("Shader {0} compiled with empty SpirV binary...");
            return false;
        }

        return true;
    }
}
