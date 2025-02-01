#pragma once

#include "Core/Functional/Function.h"
#include "AssetHandle.h"
#include "AssetRequester.h"
#include "Factories/Factory.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    
    class FAssetRequest
    {
    public:
        
        enum class EType : uint8
        {
            Invalid = 0,
            Load,
            Unload,
        };

        enum class EStage : uint8
        {
            None = 0,
            
            LoadingAsset,
            LoadingDependencies,
            
            UnloadingAsset,
            
            Complete,
        };
        
        struct FAssetRequestContext
        {
            TFunction<void(FAssetHandle&, const FAssetRequester&)>    LoadAssetCallback;
        };
        
        FAssetRequest() = default;
        FAssetRequest(const FAssetRequester& InRequester, EType InType, FAssetRecord* InRecord)
            : Requester(InRequester)
            , Type(InType)
            , Stage(EStage::None)
            , Record(InRecord)
        {}

        FORCEINLINE bool IsValid() const { return Record != nullptr; }
        FORCEINLINE bool IsActive() const { return Stage != EStage::Complete; }
        FORCEINLINE bool IsComplete() const { return Stage == EStage::Complete; }

        FORCEINLINE bool IsLoadRequest() const { return Type == EType::Load; }
        FORCEINLINE bool IsUnloadRequest() const { return Type == EType::Unload; }

        FORCEINLINE EStage GetStage() const { return Stage; }

        FORCEINLINE const FAssetRecord* GetAssetRecord() const { return Record; }
        FORCEINLINE const EAssetType GetAssetType() const { return Record->GetAssetType(); }
        FORCEINLINE const FGuid& GetGuid() const { return Guid; }
        
        bool Update(FAssetRequestContext& Context);

    

    private:

        FGuid                   Guid;
        FAssetRecord*           Record;
        FAssetRequester         Requester;
        EType                   Type;
        EStage                  Stage;
    };
}
