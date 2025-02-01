
#include "AssetManager.h"
#include "Assets/AssetRecord.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Platform/Filesystem/FileHelper.h"

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

    void FAssetManager::Update(bool bWaitForAsyncTasks)
    {
        PROFILE_SCOPE(AssetManager)

        {
            FRecursiveScopeLock ScopeLock(RecursiveMutex);

            for (FPendingRequest& PendingRequest : PendingRequests)
            {
                FAssetRequest* ActiveRequest = TryFindActiveRequest(PendingRequest.Record);

                if (PendingRequest.Type == FPendingRequest::Type::Load)
                {
                    if (ActiveRequest != nullptr)
                    {
                        if (ActiveRequest->IsUnloadRequest())
                        {
                            // Switch from unload back to load.
                        }
                    }
                    else if (PendingRequest.Record->IsLoaded())
                    {
                        // Asset was alraedy loaded (possibily this frame).
                    }
                    else
                    {
                        FAssetRequest* NewRequest = new FAssetRequest(PendingRequest.RequesterID, FAssetRequest::EType::Load, PendingRequest.Record);
                        ActiveRequests.emplace_back(NewRequest);
                    }
                }
                else /** Unload request */
                {
                    if (ActiveRequest != nullptr)
                    {
                        if (ActiveRequest->IsLoadRequest())
                        {
                            // Switch from load to unload.
                        }
                    }
                    else if (PendingRequest.Record->IsUnloaded())
                    {
                        if (!PendingRequest.Record->HasReferences())
                        {
                            auto Iter = AssetRecord.find(PendingRequest.Record->GetAssetGuid());
                            Assert(Iter != AssetRecord.end());
                            Assert(Iter->second == PendingRequest.Record);

                            delete PendingRequest.Record;
                            AssetRecord.erase(Iter);
                        }
                    }
                    else
                    {
                        FAssetRequest* UnloadRequest = new FAssetRequest(PendingRequest.RequesterID, FAssetRequest::EType::Unload, PendingRequest.Record);
                        ActiveRequests.emplace_back(UnloadRequest);
                    }
                }
            }

            PendingRequests.clear();
        }

        
        {
            for (FAssetRequest* Request : CompletedRequests)
            {

                FGuid ID = Request->Get

                
                if (Request->IsUnloadRequest())
                {
                    if (!Request->GetAssetRecord()->HasReferences())
                    {
                        auto Itr = AssetRecord.find()
                    }
                }


                delete Request;
            }

            CompletedRequests.clear();
        }
    }

    void FAssetManager::LoadAsset(FAssetHandle& InHandle, const FAssetRequester& Requester)
    {
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        FAssetRecord* Record = FindOrCreateAssetRecord(InHandle.GetAssetGuid());
        InHandle.SetRecord(Record);

        if (!Record->HasReferences())
        {
            AddPendingRequest(FPendingRequest(FPendingRequest::Type::Load, Record, Requester));
        }

        Record->AddReference(Requester);
    }

    void FAssetManager::UnloadAsset(FAssetHandle& InHandle, const FAssetRequester& Requester)
    {
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        FAssetRecord* Record = FindAssetRecordChecked(InHandle.GetAssetGuid());
        Record->RemoveReference(Requester);

        if (!Record->HasReferences())
        {
            AddPendingRequest(FPendingRequest(FPendingRequest::Type::Unload, Record, Requester));
        }
    }

    ELoadResult FAssetManager::LoadFromDisk(const FAssetHandle& InAssetHandle, const FAssetPath& InPath, FAssetRecord* InRecord)
    {
        TVector<uint8> Buffer;
        if (!FFileHelper::LoadFileToArray(Buffer, InPath.GetPathAsString()))
        {
            LOG_ERROR("Failed to read asset file: {0}", InPath.GetPathAsString().c_str());
            return ELoadResult::Failed;
        }

        FMemoryReader Reader(Buffer);

        FAssetHeader Header;
        Reader << Header;

        InRecord->Dependencies.reserve(Header.Dependencies.size());
        for (const FGuid& Dependency : Header.Dependencies)
        {
            InRecord->AddDependency(Dependency);
        }

        FFactory* Factory = FactoryRegistry.GetFactory(Header.Type);
        if (Factory != nullptr)
        {
            return Factory->CreateNew(InAssetHandle, InPath, InRecord, Reader);
        }

        return ELoadResult::Failed;
    }

    FAssetRecord* FAssetManager::FindOrCreateAssetRecord(const FGuid& InGuid)
    {
        FAssetRecord* Record = nullptr;
        auto It = AssetRecord.find(InGuid);
        if (It == AssetRecord.end())
        {
            Record = new FAssetRecord(InGuid);
            AssetRecord[InGuid] = Record;
        }
        else
        {
            Record = It->second;
        }

        return Record;
    }

    FAssetRecord* FAssetManager::FindAssetRecordChecked(const FGuid& InGuid)
    {
        Assert(InGuid.IsValid());
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        auto const Iter = AssetRecord.find(InGuid);
        Assert(Iter != AssetRecord.end());
        
        return Iter->second;
    }
    
    void FAssetManager::AddPendingRequest(FPendingRequest&& NewRequest)
    {
        FRecursiveScopeLock ScopeLock(RecursiveMutex);

        auto Predicate = [] (const FPendingRequest& Request, const FGuid& Guid) { return Request.Record->GetAssetGuid() == Guid; };
        const int32 Index = VectorFindIndex(PendingRequests, NewRequest.Record->GetAssetGuid(), Predicate);

        if (Index == -1)
        {
            PendingRequests.emplace_back(eastl::move(NewRequest));
        }
        else
        {
            PendingRequests[Index] = eastl::move(NewRequest);
        }
    }

    FAssetRequest* FAssetManager::TryFindActiveRequest(const FAssetRecord* InRecord) const
    {
        FRecursiveScopeLock Lock(RecursiveMutex);

        auto Predicate = [] (const FAssetRequest* Request, const FAssetRecord* Record) { return Request->GetAssetRecord() == Record; };
        const int32 Index = VectorFindIndex(ActiveRequests, InRecord, Predicate);

        if (Index != -1)
        {
            return ActiveRequests[Index];
        }

        return nullptr;
    }
    
    void FAssetManager::ProcessAssetRequests()
    {
        while (bAssetThreadRunning)
        {
            PROFILE_SCOPE(ProcessAssetRequests)
            
            FRecursiveScopeLock Lock(RecursiveMutex);

            for (int i = ActiveRequests.size() - 1; i >= 0; --i)
            {
                FAssetRequest::FAssetRequestContext Context;
                Context.LoadAssetCallback = [this] (FAssetHandle& InHandle, const FAssetRequester& Requester) { LoadAsset(InHandle, Requester); };
                
                bool bCompleted = false;

                FAssetRequest* Request = ActiveRequests[i];

                if (Request->IsActive())
                {
                    bCompleted = Request->Update(Context);
                }
                else
                {
                    bCompleted = true;
                }

                if (bCompleted)
                {
                    CompletedRequests.emplace_back(Request);
                    ActiveRequests.erase_unsorted(ActiveRequests.begin() + i);
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

