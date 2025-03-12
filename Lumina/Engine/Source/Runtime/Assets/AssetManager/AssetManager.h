#pragma once

#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetRequest.h"
#include "Assets/Factories/FactoryRegistry.h"
#include "Containers/Array.h"
#include "Core/Threading/Thread.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
	class FAssetRecord;
	
	class FAssetManager : public ISubsystem
	{
	public:

		FAssetManager();
		~FAssetManager() override;

		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void Update();
		
		void LoadAsset(FAssetHandle& InAsset);
		void UnloadAsset(FAssetHandle& InAsset);

		void NotifyAssetRequestCompleted(FAssetRequest* Request);

		
	private:

		FAssetRecord* FindOrCreateAssetRecord(const FAssetHandle& InAsset);
		FAssetRecord* FindAssetRecord(const FAssetHandle& InHandle);
		FAssetRequest* TryFindActiveRequest(FAssetRecord* Record);

		void ProcessAssetRequests();
	
	private:

		
		std::thread									AssetRequestThread;
		eastl::atomic<bool>							bAssetThreadRunning = true;
		mutable FRecursiveMutex						RecursiveMutex;

		FFactoryRegistry							FactoryRegistry;
		
		THashMap<FAssetPath, FAssetRecord*>			AssetRecord;

		TQueue<FAssetRequest*>						RequestQueue;
		TSet<FAssetRequest*>						ActiveRequests;

	};
	

	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
