#pragma once

#include <filesystem>
#include <shared_mutex>
#include <unordered_map>

#include "Shader.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    class FShader;

    class FShaderLibrary : public TSingleton<FShaderLibrary>
    {
    public:
        
        FShaderLibrary();
        ~FShaderLibrary();

        void Shutdown();

        bool Load(std::filesystem::path Vertex, std::filesystem::path Fragment, const LString& Tag);
        
        bool Unload(std::string Name);
        bool Reload(std::filesystem::path Name);
        bool Has(std::string Key);
        
        static TRefPtr<FShader> GetShader(const std::string& Key);
        const std::unordered_map<LString, TRefPtr<FShader>>* GetShaders() const { return &Library; }
        EShaderStage EvaluateStage(std::filesystem::path File) const;

    private:

        std::unordered_map<LString, TRefPtr<FShader>> Library;
        std::shared_mutex Mutex;
        
    };
}
