
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
        
            case ELoadStage::WaitForDependencies:
            {
               
            }
            break;

            case ELoadStage::Complete:
            {
                    
            }
            break;
            
            default:
            {
                
            }
        }


        return IsLoadingCompleted();
    }

    void FAssetLoadRequest::LoadResource(FLoadRequestCallbackContext& Context)
    {
        if (Factory->CreateNew(AssetHandle) == ELoadResult::Failed)
        {
            LOG_ERROR("Resource Request: Failed to load resource data {0}!", AssetHandle.AssetPath.GetPathAsString());
            LoadState = ELoadStage::Complete;
            AssetHandle.AssetPtr.reset();
            return;
        }

        
        
    }
}
