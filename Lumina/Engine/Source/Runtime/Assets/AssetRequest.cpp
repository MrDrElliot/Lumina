
#include "AssetRequest.h"

#include "Factories/Factory.h"

namespace Lumina
{
    bool FAssetRequest::Update(FRequestCallbackContext& Context)
    {
        switch (LoadState)
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
        ELoadResult LoadResult = Factory->CreateNew(AssetRecord);
        AssetRecord->SetLoadingState(EAssetLoadState::Loading);
        
        if (LoadResult == ELoadResult::Failed)
        {
            LOG_ERROR("Resource Request: Failed to load resource data {0}!", AssetRecord->GetAssetPath());
            LoadState = ELoadStage::Complete;
            AssetRecord->SetLoadingState(EAssetLoadState::Failed);
            return;
        }

        /** This may happen because we are waiting on RHI resource upload. */
        if (LoadResult == ELoadResult::InProgress)
        {
            LoadState = ELoadStage::LoadingResource;
            return;
        }

        /** Send our dependencies to get loaded as well, only happens once. Then they process themselves. */
        PendingDependencies = AssetRecord->AssetDependencies;
        for (FAssetHandle& Handle : PendingDependencies)
        {
            Context.LoadAssetCallback(Handle);
        }

        /** This resource is fully loaded, wait for dependencies to finish */
        LoadState = ELoadStage::WaitForDependencies;
        
    }

    void FAssetRequest::UpdateResourceFactory()
    {
        Factory->UpdateInProcessRequest(AssetRecord);
    }

    void FAssetRequest::ProcessDependencies(FRequestCallbackContext& Context)
    {
        /** Check if all of our dependencies are fully loaded */
        TInlineVector<FAssetHandle, 4> FinishedDependencies;
        for (FAssetHandle& Handle : PendingDependencies)
        {
            if (Handle.IsLoaded())
            {
                FinishedDependencies.push_back(Handle);
            }
        }

        if (FinishedDependencies.size() != PendingDependencies.size())
        {
            LoadState = ELoadStage::WaitForDependencies;
        }
        else
        {
            AssetRecord->GetAssetPtr()->PostLoadDependencies();
            LoadState = ELoadStage::Complete;
            AssetRecord->SetLoadingState(EAssetLoadState::Loaded);
            AssetRecord->GetAssetPtr()->PostLoad();
        }
    }
}
