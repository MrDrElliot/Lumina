#pragma once

#include "Lumina.h"
#include <shared_mutex>
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

        void LoadShadersInDirectory(const FString& Directory);
        
        bool Load(const FString& Vertex, const FString& Fragment, FName Tag);
        
        bool Unload(FName Key);
        bool Reload(FName Key);
        bool Has(FName Key);
        
        static TRefCountPtr<FShader> GetShader(FName Key);
        const THashMap<FName, TRefCountPtr<FShader>>& GetShaders() const { return Library; }
        EShaderStage EvaluateStage(std::filesystem::path File) const;

        
        
    private:

        THashMap<FName, TRefCountPtr<FShader>> Library;
        std::shared_mutex Mutex;
        
    };
}
