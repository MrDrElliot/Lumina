#pragma once

#include <EASTL/atomic.h>
#include "AssetHandle.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CFactory;
    
    class FAssetRequest
    {
    public:

        enum class ELoadStage : uint8
        {
            None = 0,
            LoadResource,
            LoadingResource,
            WaitForDependencies,
            Complete,
        };

        /** We use this for an anonymous way for a dependant resource to load. */
        struct FRequestCallbackContext
        {
            TFunction<void(FAssetHandle&)>	LoadAssetCallback;
        };
        
        FAssetRequest(FAssetRecord* InRecord, CFactory* InFactory)
            : AssetRecord(InRecord)
            , Factory(InFactory)
        {
            Assert(InRecord != nullptr)
            Assert(InFactory != nullptr)
        }

        FORCEINLINE FAssetRecord* GetAssetRecord() const { return AssetRecord; }
        FORCEINLINE CFactory* GetFactory() const { return Factory; }
        FORCEINLINE ELoadStage GetLoadStage() const { return LoadState; }
        FORCEINLINE bool IsLoadingCompleted() const { return LoadState == ELoadStage::Complete; }

        bool Update(FRequestCallbackContext& Context);

        /** Process initial loading and setup */
        void LoadResource(FRequestCallbackContext& Context);

        /** Some assets such as render resources may take time for RHI, so we update and wait */
        void UpdateResourceFactory();

        /** Asset dependencies are updated, and processsed */
        void ProcessDependencies(FRequestCallbackContext& Context);

        
        
    private:
        
        FAssetRecord*                   AssetRecord;
        CFactory*                       Factory;
        eastl::atomic<ELoadStage>       LoadState = ELoadStage::LoadResource;
        TVector<FAssetHandle>           PendingDependencies;

    };
    
}
