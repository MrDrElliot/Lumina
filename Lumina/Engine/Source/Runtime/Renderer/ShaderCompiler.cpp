#include "ShaderCompiler.h"

#include "Renderer/RHIIncl.h"
#include <shaderc/shaderc.hpp>

#include "Paths/Paths.h"

namespace Lumina
{

    class FShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        // Handles #include requests
        shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override
        {
            auto* result = new shaderc_include_result();
            
            // Load the file contents
            std::string fullPath = ResolveIncludePath(requested_source, requesting_source);
            std::string content = LoadFile(fullPath);
    
            if (content.empty())
            {
                LOG_WARN("Shader include file is empty: {0}", requested_source);
            }
    
            // Allocate memory for the result
            char* copied_content = new char[content.size() + 1];
            memcpy(copied_content, content.c_str(), content.size() + 1);
    
            char* copied_name = new char[fullPath.size() + 1];
            memcpy(copied_name, fullPath.c_str(), fullPath.size() + 1);
    
            result->source_name = copied_name;
            result->source_name_length = fullPath.size();
            result->content = copied_content;
            result->content_length = content.size();
            result->user_data = nullptr;
    
            return result;
        }
    
        // Cleanup after inclusion
        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete[] data->source_name;
            delete[] data->content;
            delete data;
        }
    
    private:
        std::string LoadFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            if (!file.is_open()) return "";
    
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
    
            std::string buffer(size, '\0');
            if (!file.read(buffer.data(), size)) return "";
    
            return buffer;
        }
    
        std::string ResolveIncludePath(const std::string& requested_source, const std::string& requesting_source)
        {
            std::filesystem::path RequestingPath = requesting_source;
            std::filesystem::path RequestedPath = RequestingPath.parent_path() / requested_source;
            
            return RequestedPath.string();
        }
    };
    
    EShaderCompileResult FShaderCompiler::CompileShader(FShaderStage& InStage, const FStringView& ShaderData)
    {
        shaderc::Compiler Compiler;
        shaderc::CompileOptions Options;

        ERHIInterfaceType RHIInterface = FRenderer::GetRenderAPI()->GetRHIInterfaceType();
        shaderc_target_env Target = shaderc_target_env_vulkan;
        
        switch (RHIInterface)
        {
            case ERHIInterfaceType::Vulkan:
                Target = shaderc_target_env_vulkan;
                break;
            case ERHIInterfaceType::OpenGL:
                Target = shaderc_target_env_opengl;
                break;
            case ERHIInterfaceType::DX12:
                std::unreachable();
                break;
        }

        std::string MacroDefs = {};
        shaderc_shader_kind ShaderCKind = shaderc_shader_kind::shaderc_glsl_infer_from_source;
        switch (InStage.Stage)
        {
            case EShaderStage::NONE:
                break;
            case EShaderStage::VERTEX:
                MacroDefs = "VERTEX_SHADER";
                ShaderCKind = shaderc_shader_kind::shaderc_vertex_shader;
                break;
            case EShaderStage::FRAGMENT:
                MacroDefs = "FRAGMENT_SHADER";
                ShaderCKind = shaderc_shader_kind::shaderc_fragment_shader;
                break;
            case EShaderStage::COMPUTE:
                MacroDefs = "COMPUTE_SHADER";
                break;
        }

        
        Options.SetOptimizationLevel(shaderc_optimization_level_performance);
        Options.SetTargetEnvironment(Target, 0);
        Options.AddMacroDefinition(MacroDefs);
        Options.SetGenerateDebugInfo();
        Options.SetAutoBindUniforms(true);
        Options.SetAutoMapLocations(true);
        Options.SetIncluder(std::make_unique<FShaderIncluder>());
        

        // Preprocess shader
        auto PreprocessResult = Compiler.PreprocessGlsl(ShaderData.data(), ShaderData.size(), ShaderCKind, InStage.RawPath.c_str(), Options);
        if(PreprocessResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_ERROR("Failed to Preprocess shader for reason: {0}", PreprocessResult.GetErrorMessage());
            return EShaderCompileResult::Failed;
        }

        FString Preprocess = FString(PreprocessResult.begin(), PreprocessResult.end());

        Options.SetOptimizationLevel(shaderc_optimization_level_performance);
        Options.SetTargetEnvironment(Target, 0);
        Options.AddMacroDefinition(MacroDefs);
        Options.SetGenerateDebugInfo();
        Options.SetAutoBindUniforms(true);
        Options.SetAutoMapLocations(true);
        Options.SetIncluder(std::make_unique<FShaderIncluder>());
        
        // Compile to SPIR-V
        auto CompileResult = Compiler.CompileGlslToSpv(Preprocess.c_str(), Preprocess.size(), ShaderCKind, InStage.RawPath.c_str(), Options);

        if(CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_ERROR("Failed to compile shader for reason: {0}", CompileResult.GetErrorMessage());
            return EShaderCompileResult::Failed;
        }

        InStage.ShaderString = FMemory::Move(Preprocess);
        InStage.Binaries.assign(CompileResult.begin(), CompileResult.end());

        return EShaderCompileResult::Success;
    }
}
