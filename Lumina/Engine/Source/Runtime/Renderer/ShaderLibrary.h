#pragma once

#include <filesystem>
#include <shared_mutex>
#include <unordered_map>
#include <EASTL/unordered_map.h>

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

        bool Load(std::filesystem::path Vertex, std::filesystem::path Fragment, const FString& Tag);
        
        bool Unload(FString Name);
        bool Reload(std::filesystem::path Name);
        bool Has(FString Key);
        
        static TRefPtr<FShader> GetShader(const FString& Key);
        const eastl::unordered_map<FString, TRefPtr<FShader>>* GetShaders() const { return &Library; }
        EShaderStage EvaluateStage(std::filesystem::path File) const;

    private:

        eastl::unordered_map<FString, TRefPtr<FShader>> Library;
        std::shared_mutex Mutex;
        
    };
}
