#include "ShaderLibrary.h"

#include <fstream>

#include "ShaderCompiler.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    FShaderLibrary* FShaderLibrary::Instance = nullptr;
    FShaderCompiler Compiler;
    
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
        Compiler.AddGlobalMacro("_LUMINA_SCENE_DESCRIPTOR_SET", "0");
        Compiler.AddGlobalMacro("_LUMINA_PASS_DESCRIPTOR_SET", "1");
        Compiler.AddGlobalMacro("_LUMINA_DRAW_CALL_DESCRIPTOR_SET", "2");
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

        FShaderCompilationResult compilation_result = Compiler.Compile(shader_source, Path.filename().string());

        if (!compilation_result.bValid) return false;

        std::shared_ptr<FShader> Shader = FShader::Create(compilation_result.Bytecode, Path);

        Mutex.lock();
        Library.emplace(Path.filename().string(), Shader);
        Mutex.unlock();

        return true;
    }

    bool FShaderLibrary::Unload(std::string Name)
    {
        if (Library.find(Name) == Library.end()) return false;

        Library.find(Name)->second->Destroy();
        Library.erase(Name);

        return true;
    }

    bool FShaderLibrary::Reload(std::filesystem::path Name)
    {
        return false;
    }

    bool FShaderLibrary::Has(std::string Key)
    {
        std::scoped_lock<std::shared_mutex> lock(Mutex);
        return Library.find(Key) != Library.end();
    }

    std::shared_ptr<FShader> FShaderLibrary::GetShader(std::string Key)
    {
        return Library.find(Key)->second;
    }

    EShaderStage FShaderLibrary::EvaluateStage(std::filesystem::path File) const
    {
        if (File.extension().string() == ".vert")
            return EShaderStage::VERTEX;
        if (File.extension().string() == ".frag")
            return EShaderStage::FRAGMENT;
        if (File.extension().string() == ".comp")
            return EShaderStage::COMPUTE;

        return EShaderStage::UNKNOWN;
    }
}
