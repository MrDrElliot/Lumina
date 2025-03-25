
#include "AssetManager.h"

#include "Core/Math/Hash/Hash.h"
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
        
        rpmalloc_thread_finalize(1);
        AssetRecord.clear();
    }

    void FAssetManager::Update()
    {
    }

    FAssetRecord* FAssetManager::LoadAsset(FAssetHandle& InAsset)
    {
        FAssetRecord* FoundRecord = FindOrCreateAssetRecord(InAsset);
        Assert(FoundRecord);
        InAsset.AssetRecord = FoundRecord;
        
        FAssetRequest* ActiveRequest = TryFindActiveRequest(FoundRecord);
        Assert(ActiveRequest);
        
        RequestQueue.push(ActiveRequest);

        return FoundRecord;
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
        FMemory::Delete(Request);
    }

    FAssetRecord* FAssetManager::FindOrCreateAssetRecord(const FAssetHandle& InAsset)
    {
        Assert(InAsset.IsSet());
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        FAssetRecord* Record;
        auto const Itr = AssetRecord.find(InAsset.GetAssetPath());
        if (Itr == AssetRecord.end())
        {
            Record = FMemory::New<FAssetRecord>(InAsset.GetAssetPath(), InAsset.GetAssetType());
            AssetRecord.emplace(InAsset.GetAssetPath(), Record);
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
        auto It = eastl::find_if(ActiveRequests.begin(), ActiveRequests.end(), [Record](const FAssetRequest* Request)
        {
            return Request->GetAssetRecord() == Record;
        });

        if (It != ActiveRequests.end())
        {
            return *It;
        }

        FFactory* Factory = FactoryRegistry.GetFactory(Record->GetAssetType());
        if (!Factory) 
        {
            return nullptr;
        }

        FAssetRequest* NewRequest = FMemory::New<FAssetRequest>(Record, Factory);
        ActiveRequests.push_back(NewRequest);

        return NewRequest;
    }


    void FAssetManager::ProcessAssetRequests()
    {
        rpmalloc_thread_initialize();
        
        while (bAssetThreadRunning)
        {
            FAssetRequest::FRequestCallbackContext Context;
            Context.LoadAssetCallback = [this] (FAssetHandle& Handle) { LoadAsset(Handle); };
            
            while (!RequestQueue.empty())
            {
                FAssetRequest* Request = RequestQueue.front();
                RequestQueue.pop();
                
                if (Request->Update(Context))
                {
                    NotifyAssetRequestCompleted(Request);
                }
                
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

