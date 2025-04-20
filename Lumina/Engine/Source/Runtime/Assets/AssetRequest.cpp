
#include "AssetRequest.h"
#include "Factories/Factory.h"

namespace Lumina
{
    bool FAssetRequest::Update(FRequestCallbackContext& Context)
    {
        switch (LoadState.load())
        {
            case ELoadStage::LoadResource:
            {
                LoadResource(Context);
            }
            break;

            case ELoadStage::LoadingResource:
            {
                UpdateResourceFactory();
            }
        
            case ELoadStage::WaitForDependencies:
            {
               ProcessDependencies(Context);
            }
            break;
        }

        /** If we haven't completed or failed, request a process next update */
        if (LoadState != ELoadStage::Complete)
        {
            FAssetHandle Handle(AssetRecord->GetAssetPath(), AssetRecord->GetAssetType());
            Context.LoadAssetCallback(Handle);
            return false;
        }
        else
        {
            return true;
        }
    }

    void FAssetRequest::LoadResource(FRequestCallbackContext& Context)
    {
        ELoadResult LoadResult = Factory->LoadFromDisk(AssetRecord);
        AssetRecord->SetLoadingState(EAssetLoadState::Loading);
        
        if (LoadResult == ELoadResult::Failed)
        {
            LOG_ERROR("Resource Request: Failed to load resource data {0}!", AssetRecord->GetAssetPath());
            LoadState.store(ELoadStage::Complete, eastl::memory_order_relaxed);
            AssetRecord->SetLoadingState(EAssetLoadState::Failed);
            return;
        }

        /** This may happen because we are waiting on RHI resource upload. */
        if (LoadResult == ELoadResult::InProgress)
        {
            LoadState.store(ELoadStage::LoadingResource, eastl::memory_order_relaxed);
            return;
        }

        /** Send our dependencies to get loaded as well, only happens once. Then they process themselves. */
        PendingDependencies = AssetRecord->AssetDependencies;
        for (FAssetHandle& Handle : PendingDependencies)
        {
            Context.LoadAssetCallback(Handle);
        }

        /** This resource is fully loaded, wait for dependencies to finish */
        LoadState.store(ELoadStage::WaitForDependencies, eastl::memory_order_relaxed);
        
    }

    void FAssetRequest::UpdateResourceFactory()
    {
        Factory->UpdateInProcessRequest(AssetRecord);
    }

    void FAssetRequest::ProcessDependencies(FRequestCallbackContext& Context)
    {
        /** Check if all of our dependencies are fully loaded */
        TFixedVector<FAssetHandle, 4> FinishedDependencies;
        for (FAssetHandle& Handle : PendingDependencies)
        {
            if (Handle.IsLoaded())
            {
                FinishedDependencies.push_back(Handle);
            }
        }

        if (FinishedDependencies.size() != PendingDependencies.size())
        {
            LoadState.store(ELoadStage::WaitForDependencies, eastl::memory_order_relaxed);
        }
        else
        {
            AssetRecord->GetAssetPtr()->PostLoadDependencies();
            LoadState.store(ELoadStage::Complete, eastl::memory_order_relaxed);
            AssetRecord->SetLoadingState(EAssetLoadState::Loaded);
            AssetRecord->GetAssetPtr()->PostLoad();
        }
    }
}
