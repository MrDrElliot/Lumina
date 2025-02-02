
#include "AssetManager.h"

namespace Lumina
{
    FAssetManager::FAssetManager()
    {
        
    }

    FAssetManager::~FAssetManager()
    {
    }

    void FAssetManager::Initialize()
    {
        AssetRequestThread = std::thread(&FAssetManager::ProcessAssetRequests, this);
    }

    void FAssetManager::Deinitialize()
    {
        if (AssetRequestThread.joinable())
        {
            bAssetThreadRunning.exchange(false);
            AssetRequestThread.join();
        }
        
    }

    void FAssetManager::LoadAsset(FAssetHandle& Asset)
    {
        FAssetLoadRequest* ActiveRequest = FindOrCreateRequest(Asset);
        LoadingQueue.push(ActiveRequest);
    }

    void FAssetManager::NotifyAssetRequestCompleted(FAssetLoadRequest* Request)
    {
        
    }

    FAssetLoadRequest* FAssetManager::FindOrCreateRequest(FAssetHandle& Asset)
    {
        FAssetLoadRequest* NewRequest = nullptr;
        auto It = eastl::find(ActiveLoadRequests.begin(), ActiveLoadRequests.end(), Asset);
        if (It == ActiveLoadRequests.end())
        {
            EAssetType AssetType = Asset.AssetType;
            FFactory* Factory = FactoryRegistry.GetFactory(AssetType);
            
            NewRequest = new FAssetLoadRequest(Asset, Factory);
        }
        else
        {
            NewRequest = *It;
        }

        return NewRequest;
    }

    void FAssetManager::ProcessAssetRequests()
    {
        while (bAssetThreadRunning)
        {
            PROFILE_SCOPE(ProcessAssetRequests)
            
            FRecursiveScopeLock ScopeLock(RecursiveMutex);

            FAssetLoadRequest::FLoadRequestCallbackContext Context;
            Context.LoadAssetCallback = [this] (FAssetHandle& Handle) { LoadAsset(Handle); };

            
            while (!LoadingQueue.empty())
            {
                FAssetLoadRequest* Request = LoadingQueue.front();
                
                if (Request->Update(Context))
                {
                    NotifyAssetRequestCompleted(Request);
                }
                
                LoadingQueue.pop();
            }

            

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

