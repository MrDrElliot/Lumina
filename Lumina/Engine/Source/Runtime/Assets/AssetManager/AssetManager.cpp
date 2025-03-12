
#include "AssetManager.h"

#include "Core/Performance/PerformanceTracker.h"

namespace Lumina
{
    FAssetManager::FAssetManager()
    {
        
    }

    FAssetManager::~FAssetManager()
    {
    }

    void FAssetManager::Initialize(FSubsystemManager& Manager)
    {
        AssetRequestThread = std::thread(&FAssetManager::ProcessAssetRequests, this);
    }

    void FAssetManager::Deinitialize()
    {
        bAssetThreadRunning.exchange(false);
        if (AssetRequestThread.joinable())
        {
            AssetRequestThread.join();
        }
    }

    void FAssetManager::Update()
    {
    }

    void FAssetManager::LoadAsset(FAssetHandle& InAsset)
    {
        FAssetRecord* FoundRecord = FindOrCreateAssetRecord(InAsset);
        Assert(FoundRecord);
        InAsset.AssetRecord = FoundRecord;

        if (FoundRecord->IsLoaded() && FoundRecord->GetAssetPtr() != nullptr && FoundRecord->GetReferenceCount() > 0)
        {
            FoundRecord->AddRef();
        }
        
        FAssetRequest* ActiveRequest = TryFindActiveRequest(FoundRecord);
        Assert(ActiveRequest);
        
        RequestQueue.push(ActiveRequest);
    }

    void FAssetManager::UnloadAsset(FAssetHandle& InAsset)
    {
        FAssetRecord* FoundRecord = FindAssetRecord(InAsset);

        if (FoundRecord->GetReferenceCount() > 0)
        {
            FoundRecord->Release();
        }
        else
        {
            FoundRecord->SetLoadingState(EAssetLoadState::Unloaded);
            FoundRecord->FreeAssetMemory();
        }
    }

    void FAssetManager::NotifyAssetRequestCompleted(FAssetRequest* Request)
    {
        

        delete Request;
    }

    FAssetRecord* FAssetManager::FindOrCreateAssetRecord(const FAssetHandle& InAsset)
    {
        Assert(InAsset.IsSet());
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        FAssetRecord* Record;
        auto const Itr = AssetRecord.find(InAsset.GetAssetPath());
        if (Itr == AssetRecord.end())
        {
            Record = new FAssetRecord(InAsset.GetAssetPath(), InAsset.GetAssetType());
        }
        else
        {
            Record = Itr->second;
        }

        return Record;
        
    }

    FAssetRecord* FAssetManager::FindAssetRecord(const FAssetHandle& InHandle)
    {
        Assert(InHandle.IsSet());
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        auto const Itr = AssetRecord.find(InHandle.GetAssetPath());
        Assert(Itr != AssetRecord.end());

        return Itr->second;
    }

    FAssetRequest* FAssetManager::TryFindActiveRequest(FAssetRecord* Record)
    {
        FAssetRequest* NewRequest = nullptr;
        auto It = eastl::find_if(ActiveRequests.begin(), ActiveRequests.end(), [Record] (const FAssetRequest* Request)
        {
            return Request->GetAssetRecord() == Record;
        });
        
        if (It == ActiveRequests.end())
        {
            
            FFactory* Factory = FactoryRegistry.GetFactory(Record->GetAssetType());
            NewRequest = new FAssetRequest(Record, Factory);
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
            
            FAssetRequest::FRequestCallbackContext Context;
            Context.LoadAssetCallback = [this] (FAssetHandle& Handle) { LoadAsset(Handle); };
            
            while (!RequestQueue.empty())
            {
                FAssetRequest* Request = RequestQueue.front();
                
                if (Request->Update(Context))
                {
                    NotifyAssetRequestCompleted(Request);
                }
                
                RequestQueue.pop();
            }

            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

