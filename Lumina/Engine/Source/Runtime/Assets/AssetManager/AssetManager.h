#pragma once

#include "EASTL/atomic.h"
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

		FAssetRequest* TryFindActiveRequest(const FString& InAssetPath);

		void ProcessAssetRequests();
	
	private:

		
		std::thread												AssetRequestThread;
		eastl::atomic<bool>										bAssetThreadRunning = true;
		mutable FRecursiveMutex									RecursiveMutex;
		
		TQueue<FAssetRequest*>									RequestQueue;
		TVector<FAssetRequest*>									ActiveRequests;

		FMutex													RequestMutex;
		std::mutex												FlushMutex;
		std::condition_variable									FlushCV;
		
	};
	

	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
