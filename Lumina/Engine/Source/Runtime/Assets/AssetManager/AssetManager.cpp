
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

    void FAssetManager::Initialize(FSubsystemManager& Manager)
    {
    }

    void FAssetManager::Deinitialize()
    {
        FlushAsyncLoading();
    }

    FAssetRequest* FAssetManager::LoadAsset(const FString& InAssetPath)
    {
        bool bAlreadyInQueue;
        FAssetRequest* ActiveRequest = TryFindActiveRequest(InAssetPath, bAlreadyInQueue);
        
        if (bAlreadyInQueue)
        {
            return ActiveRequest;
        }
        
        SubmitAssetRequest(ActiveRequest);
        return ActiveRequest;
    }

    void FAssetManager::NotifyAssetRequestCompleted(FAssetRequest* Request)
    {
        auto It = eastl::find(ActiveRequests.begin(), ActiveRequests.end(), Request);
        Assert(It != ActiveRequests.end());
        ActiveRequests.erase(It);
        
        Memory::Delete(Request);
    }

    void FAssetManager::FlushAsyncLoading()
    {
        std::unique_lock Lock(FlushMutex);
        FlushCV.wait(Lock, [this]
        {
            return OutstandingTasks.load() == 0;
        });
    }

    FAssetRequest* FAssetManager::TryFindActiveRequest(const FString& InAssetPath, bool& bAlreadyInQueue)
    {
        FAssetRequest** It = eastl::find_if(ActiveRequests.begin(), ActiveRequests.end(), [InAssetPath](const FAssetRequest* Request)
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
                    Manager->NotifyAssetRequestCompleted(Request);
                }

                {
                    FScopeLock Lock(Manager->FlushMutex);
                    --Manager->OutstandingTasks;
                    Manager->FlushCV.notify_all();
                }
            }
        };

        {
            FScopeLock Lock(FlushMutex);
            ++OutstandingTasks;
        }

        auto* Task = Memory::New<FAssetTask>(this, Request);
        FTaskSystem::Get()->ScheduleTask(Task);
    }
    
}

