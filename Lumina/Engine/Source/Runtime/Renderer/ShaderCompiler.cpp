#include "ShaderCompiler.h"

#include "Renderer/RHIIncl.h"
#include <shaderc/shaderc.hpp>

namespace Lumina
{
    EShaderCompileResult FShaderCompiler::CompileShader(TVector<uint32>& OutBinaries, const FStringView& ShaderData, const FStringView& FileName)
    {
        shaderc::Compiler Compiler;
        shaderc::CompileOptions Options;

        Options.SetOptimizationLevel(shaderc_optimization_level_performance);
        Options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);
        Options.SetGenerateDebugInfo();

        auto Result = Compiler.CompileGlslToSpv
        (
            ShaderData.data(),
            ShaderData.size(), 
            shaderc_glsl_infer_from_source, 
            FileName.data(),
            Options
        );

        if(Result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_ERROR("Failed to compile shader for reason: {0}", Result.GetErrorMessage());
            return EShaderCompileResult::Failed;
        }

        OutBinaries.assign(Result.begin(), Result.end());

        return EShaderCompileResult::Success;
    }
}
