#pragma once

#include "AssetHandle.h"
#include "EASTL/internal/atomic/atomic.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FFactory;
}

namespace Lumina
{
    class FAssetLoadRequest
    {
    public:

        enum class ELoadStage : uint8
        {
            None = 0,
            LoadResource,
            WaitForDependencies,
            Complete,
        };

        /** We use this for an anonymous way for a dependant resource to load. */
        struct FLoadRequestCallbackContext
        {
            TFunction<void(FAssetHandle&)>	LoadAssetCallback;
        };
        
        FAssetLoadRequest(const FAssetHandle& InHandle, FFactory* InFactory)
            : AssetHandle(InHandle)
            , Factory(InFactory)
        {
            Assert(AssetHandle.IsSet());
        }
        
        FORCEINLINE ELoadStage GetLoadStage() const { return LoadState; }
        FORCEINLINE bool IsLoadingCompleted() const { return LoadState == ELoadStage::Complete; }
        FORCEINLINE const FAssetHandle& GetHandle() const { return AssetHandle; }

        bool Update(FLoadRequestCallbackContext& Context);

        void LoadResource(FLoadRequestCallbackContext& Context);

        
        
    private:
        
        FAssetHandle                    AssetHandle;
        FFactory*                       Factory;
        eastl::atomic<ELoadStage>       LoadState = ELoadStage::LoadResource;
        TVector<FAssetHandle>           FinishedDependencies;
        TVector<FAssetHandle>           PendingDependencies;

    };
    
}
