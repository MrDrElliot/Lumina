#pragma once
#include <map>

#include "Shader.h"
namespace Lumina
{
    struct FShaderCompilationResult
    {
        std::map<EShaderStage, std::vector<glm::uint32>> Bytecode;
        bool bValid;

    };

    class FShaderCompiler
    {
    public:
        
        FShaderCompiler();
        
        bool ReadShaderFile(std::filesystem::path path, std::stringstream* out);
        bool Compile(const std::stringstream& source, const EShaderStage& Stage, std::string_view Filename, std::vector<glm::uint32>* Out);
        void AddGlobalMacro(const std::string& key, const std::string& value);

        FShaderCompilationResult Compile(std::string& source, const std::string& filename);

    private:

        shaderc_c
        shaderc::Compiler m_Compiler;
        shaderc::CompileOptions m_GlobalOptions;
        std::map<std::string, std::string> m_GlobalMacros;

    };
}
