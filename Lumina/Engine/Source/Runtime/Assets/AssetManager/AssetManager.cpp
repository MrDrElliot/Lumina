
#include "AssetManager.h"

#include "TaskScheduler.h"
#include "Core/Math/Hash/Hash.h"
#include "TaskSystem/TaskSystem.h"

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
    }

    void FAssetManager::Deinitialize()
    {
        FlushAsyncLoading();
    }

    FAssetRequest* FAssetManager::LoadAsset(const FString& InAssetPath)
    {
        bool bAlreadyInQueue = false;
        FAssetRequest* ActiveRequest = TryFindActiveRequest(InAssetPath, bAlreadyInQueue);
        
        if (!bAlreadyInQueue)
        {
            SubmitAssetRequest(ActiveRequest);
        }

        return ActiveRequest;
    }

    void FAssetManager::NotifyAssetRequestCompleted(FAssetRequest* Request)
    {
        auto It = eastl::find(ActiveRequests.begin(), ActiveRequests.end(), Request);
        Assert(It != ActiveRequests.end())
        ActiveRequests.erase(It);
        Memory::Delete(Request);
    }

    void FAssetManager::FlushAsyncLoading()
    {
        while (OutstandingTasks.load(std::memory_order_acquire) != 0)
        {
            std::this_thread::yield();
        }
    }

    FAssetRequest* FAssetManager::TryFindActiveRequest(const FString& InAssetPath, bool& bAlreadyInQueue)
    {
        auto It = eastl::find_if(ActiveRequests.begin(), ActiveRequests.end(), [&](const FAssetRequest* Request)
        {
            return Request->GetAssetPath() == InAssetPath;
        });

        if (It != ActiveRequests.end())
        {
            bAlreadyInQueue = true;
            return *It;
        }

        bAlreadyInQueue = false;
        FAssetRequest* NewRequest = Memory::New<FAssetRequest>(InAssetPath);
        ActiveRequests.push_back(NewRequest);
        return NewRequest;
        
    }

    void FAssetManager::SubmitAssetRequest(FAssetRequest* Request)
    {
        ++OutstandingTasks;

        struct FAssetTask : ITaskSet
        {
            FAssetManager* Manager;
            FAssetRequest* Request;

            FAssetTask(FAssetManager* InManager, FAssetRequest* InRequest)
                : Manager(InManager), Request(InRequest)
            {
                m_SetSize = 1;
            }

            void ExecuteRange(enki::TaskSetPartition range, uint32_t threadnum) override
            {
                if (Request->Process())
                {
                    for (auto& Functor : Request->Listeners)
                    {
                        Functor(Request->PendingObject);
                    }
                    
                    Manager->NotifyAssetRequestCompleted(Request);
                }

                Manager->OutstandingTasks.fetch_sub(1, std::memory_order_release);
            }
        };

        auto* Task = Memory::New<FAssetTask>(this, Request);
        Request->SetTask(Task);
        GTaskSystem->ScheduleTask(Task);
    }
    
}

