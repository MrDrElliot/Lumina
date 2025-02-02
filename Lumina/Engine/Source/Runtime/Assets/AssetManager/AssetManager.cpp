
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
        auto It = eastl::find(AssetsInRequestQueue.begin(), AssetsInRequestQueue.end(), Asset);
        if (It == AssetsInRequestQueue.end())
        {
            
            LoadRequests.push(Asset);
        }
    }

    FAssetLoadRequest* FAssetManager::FindOrCreateRequest(FAssetHandle& Asset)
    {
        FAssetLoadRequest* NewRequest = nullptr;
        auto It = eastl::find(ActiveLoadRequests.begin(), ActiveLoadRequests.end(), Asset);
        if (It == ActiveLoadRequests.end())
        {
            
        }
    }

    void FAssetManager::ProcessAssetRequests()
    {
        while (bAssetThreadRunning)
        {
            PROFILE_SCOPE(ProcessAssetRequests)
            
            FRecursiveScopeLock ScopeLock(RecursiveMutex);
            

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    FAssetLoadRequest* FAssetManager::TryFindActiveRequest(const FAssetHandle& Handle)
    {
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        auto Predicate = [] (const FAssetLoadRequest* Request, const FAssetHandle& Handle) { return Request->GetHandle() == Handle; };
        int32 Index = VectorFindIndex(ActiveLoadRequests, Handle, Predicate);

        if (Index != -1)
        {
            return ActiveLoadRequests[Index];
        }

        return nullptr;
    }
}

