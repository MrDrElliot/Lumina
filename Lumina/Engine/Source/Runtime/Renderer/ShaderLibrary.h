#pragma once
#include <filesystem>
#include <shared_mutex>
#include <unordered_map>

#include "Shader.h"

namespace Lumina
{


    class FShaderCompiler;
    class FShader;

    class FShaderLibrary
    {
    public:
        
        FShaderLibrary();
        ~FShaderLibrary();

        static void Init();
        static void Destroy();
        static FShaderLibrary& Get() { return *Instance; }

        bool Load(std::filesystem::path Vertex, std::filesystem::path Fragment, const std::string& Tag);
        bool Unload(std::string Name);
        bool Reload(std::filesystem::path Name);
        bool Has(std::string Key);
        
        static std::shared_ptr<FShader> GetShader(std::string Key);
        const std::unordered_map<std::string, std::shared_ptr<FShader>>* GetShaders() const { return &Library; }
        EShaderStage EvaluateStage(std::filesystem::path File) const;

    private:

        static FShaderLibrary* Instance;
        static FShaderCompiler Compiler;
        std::unordered_map<std::string, std::shared_ptr<FShader>> Library;
        std::shared_mutex Mutex;
        
    };
}
