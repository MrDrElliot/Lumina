#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetRequest.h"
#include "Containers/Array.h"
#include "Core/Threading/Thread.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
	class FAssetRecord;
	
	class LUMINA_API FAssetManager : public ISubsystem
	{
	public:

		FAssetManager();
		~FAssetManager() override;

		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void Update();
		
		FAssetRequest* LoadAsset(const FString& InAssetPath);

		void NotifyAssetRequestCompleted(FAssetRequest* Request);

		void FlushAsyncLoading();
		
	private:

		FAssetRecord* FindAssetRecord(const FAssetHandle& InHandle);
		FAssetRequest* TryFindActiveRequest(const FString& InAssetPath);

		void ProcessAssetRequests();
	
	private:

		
		std::thread												AssetRequestThread;
		eastl::atomic<bool>										bAssetThreadRunning = true;
		mutable FRecursiveMutex									RecursiveMutex;
		
		THashMap<FAssetPath, TRefCountPtr<FAssetRecord>>		AssetRecord;

		TQueue<FAssetRequest*>									RequestQueue;
		TVector<FAssetRequest*>									ActiveRequests;
		
		std::mutex												FlushMutex;
		std::condition_variable									FlushCV;
		
	};
	

	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
