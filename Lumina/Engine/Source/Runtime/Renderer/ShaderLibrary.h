#pragma once

#include "Lumina.h"
#include <shared_mutex>
#include "Shader.h"
#include "Core/Singleton/Singleton.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    class FShader;

    class FShaderLibrary : public TSingleton<FShaderLibrary>
    {
    public:

        struct FShaderLoadRequest : ITaskSet
        {

            FShaderLoadRequest(const FString& InPath, const FName& InTag)
                : Path(InPath)
                , Tag(InTag)
                , LoadStatus(EStatus::Failed)
            {}
            
            void ExecuteRange(TaskSetPartition range_, uint32_t threadnum_) override
            {
                FShaderLibrary::Get()->Load(Path, Tag);
            }

            enum class EStatus : uint8
            {
                Completed,
                Failed,
            } LoadStatus;
            
            FString Path;
            FName Tag;
            
        };

        //----------------------------------------------------------------------------------
        
        FShaderLibrary();

        void Shutdown();

        void LoadShadersInDirectory(const FString& Directory);
        
        bool Load(const FString& Shader, FName Tag);
        
        bool Unload(FName Key);
        bool Reload(FName Key);
        bool Has(FName Key);
        
        FRHIShader GetShader(FName Key);
        const THashMap<FName, FRHIShader>& GetShaders() const { return Library; }
        EShaderStage EvaluateStage(std::filesystem::path File) const;

        
        
    private:

        TQueue<FShaderLoadRequest> LoadRequests;
        
        THashMap<FName, FRHIShader> Library;
        std::shared_mutex Mutex;
        
    };
}
