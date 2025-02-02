
#include "AssetRequest.h"

#include "Factories/Factory.h"

namespace Lumina
{
    bool FAssetLoadRequest::Update(FLoadRequestCallbackContext& Context)
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
            
            default:
            {
                std::unreachable();
            }
        }

        /** If we haven't completed or failed, request a process next update */
        if (LoadState < ELoadStage::WaitForDependencies)
        {
            Context.LoadAssetCallback(AssetHandle);
            return false;
        }
        else
        {
            return true;
        }
    }

    void FAssetLoadRequest::LoadResource(FLoadRequestCallbackContext& Context)
    {
        
        ELoadResult LoadResult = Factory->CreateNew(AssetHandle);
        if (LoadResult == ELoadResult::Failed)
        {
            LOG_ERROR("Resource Request: Failed to load resource data {0}!", AssetHandle.AssetPath.GetPathAsString());
            LoadState = ELoadStage::Complete;
            AssetHandle.AssetPtr.reset();
            return;
        }

        /** This may happen because we are waiting on RHI resource upload. */
        if (LoadResult == ELoadResult::InProgress)
        {
            LoadState = ELoadStage::LoadingResource;
        }
        
    }

    void FAssetLoadRequest::UpdateResourceFactory()
    {
        Factory->UpdateInProcessRequest(AssetHandle);
    }

    void FAssetLoadRequest::ProcessDependencies(FLoadRequestCallbackContext& Context)
    {
        /** Check if all of our dependencies are fully loaded */
        TVector<FAssetHandle> FinishedDependencies;
        for (FAssetHandle& Handle : PendingDependencies)
        {
            if (Handle.IsLoaded())
            {
                FinishedDependencies.push_back(Handle);
            }
            else
            {
                Context.LoadAssetCallback(Handle);
            }
        }

        if (FinishedDependencies.size() != PendingDependencies.size())
        {
            LoadState = ELoadStage::WaitForDependencies;
        }
        else
        {
            LoadState = ELoadStage::Complete;
        }
    }
}
