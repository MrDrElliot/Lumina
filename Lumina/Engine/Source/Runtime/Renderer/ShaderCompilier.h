#pragma once

#include <filesystem>
#include <map>
#include <glm/glm.hpp>

#include "ThirdParty/shaderc/glslc/file_compiler.h"
namespace Lumina
{
    enum class EShaderStage : glm::uint32;

    struct FShaderCompilationResult
    {
        std::map<EShaderStage, std::vector<glm::uint32>> bytecode;
        bool valid;

    };

    class FShaderCompiler
    {
    public:
        
        FShaderCompiler();
        
        bool ReadShaderFile(std::filesystem::path path, std::stringstream* out);
        void AddGlobalMacro(const std::string& key, const std::string& value);
        FShaderCompilationResult Compile(std::string& source, const std::string& filename);

    private:
        shaderc::Compiler Compiler;
        shaderc::CompileOptions GlobalOptions;
        std::map<std::string, std::string> GlobalMacros;

    };

}
