
#include "Source/pch.h"
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
        
    }

    void FAssetManager::Update()
    {
    }

    FAssetRequest* FAssetManager::LoadAsset(const FString& InAssetPath)
    {
        FAssetRequest* ActiveRequest = TryFindActiveRequest(InAssetPath);
        Assert(ActiveRequest)
        
        RequestQueue.push(ActiveRequest);

        return ActiveRequest;
    }

    void FAssetManager::NotifyAssetRequestCompleted(FAssetRequest* Request)
    {
        Memory::Delete(Request);
    }

    void FAssetManager::FlushAsyncLoading()
    {
        std::unique_lock<std::mutex> lock(FlushMutex);
        FlushCV.wait(lock, [this] { return RequestQueue.empty(); });
    }

    FAssetRequest* FAssetManager::TryFindActiveRequest(const FString& InAssetPath)
    {
        FAssetRequest** It = eastl::find_if(ActiveRequests.begin(), ActiveRequests.end(), [InAssetPath](const FAssetRequest* Request)
        {
            return Request->GetAssetPath() == InAssetPath;
        });

        if (It != ActiveRequests.end())
        {
            return *It;
        }
        
        FAssetRequest* NewRequest = Memory::New<FAssetRequest>(InAssetPath);
        ActiveRequests.push_back(NewRequest);
        return NewRequest;
        
    }


    void FAssetManager::ProcessAssetRequests()
    {
        Threading::InitializeThreadHeap();
        
        while (bAssetThreadRunning)
        {
            while (!RequestQueue.empty())
            {
                FAssetRequest* Request = RequestQueue.front();
                RequestQueue.pop();
                
                if (Request->Process())
                {
                    NotifyAssetRequestCompleted(Request);
                }
                else if (!Request->bFailed)
                {
                    RequestQueue.push(Request);
                }

                FlushCV.notify_all();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        Threading::ShutdownThreadHeap();
    }
}

