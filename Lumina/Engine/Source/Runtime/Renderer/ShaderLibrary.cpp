#include "ShaderLibrary.h"

#include <fstream>

#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    FShaderLibrary* FShaderLibrary::Instance = nullptr;
    
    FShaderLibrary::FShaderLibrary()
    {
    }

    FShaderLibrary::~FShaderLibrary()
    {
        for (auto& [Key,Shader] : Library)
        {
            Shader->Destroy();
        }
    }

    void FShaderLibrary::Init()
    {
        Instance = new FShaderLibrary;
    }

    void FShaderLibrary::Destroy()
    {
        delete Instance;
    }

    bool FShaderLibrary::Load(std::filesystem::path Path)
    {
        bool Result = Library.find(Path.filename().string()) != Library.end();

        if (Result)
        {
            LE_LOG_INFO("Shader \"{0}\" is already loaded.", Path.filename().string());
            return true;
        }

        Result = std::filesystem::exists(Path);

        if (!Result)
        {
            LE_LOG_INFO("Shader directory not found: {0}", Path.string());
            return false;
        }

        std::string shader_source;
        std::string line;
        std::ifstream input_stream(Path);
        while (std::getline(input_stream, line)) shader_source.append(line + '\n');

        ShaderCompilationResult compilation_result = m_Compiler.Compile(shader_source, Path.filename().string());

        if (!compilation_result.valid) return false;

        std::shared_ptr<FShader> Shader = FShader::Create(compilation_result.bytecode, Path);

        Mutex.lock();
        Library.emplace(Path.filename().string(), Shader);
        Mutex.unlock();
    }

    bool FShaderLibrary::Unload(std::string Name)
    {
    }

    bool FShaderLibrary::Reload(std::filesystem::path Name)
    {
    }

    bool FShaderLibrary::Has(std::string Key)
    {
    }

    std::shared_ptr<FShader> FShaderLibrary::GetShader(std::string Key)
    {
    }

    EShaderStage FShaderLibrary::EvaluateStage(std::filesystem::path File) const
    {
    }
}
