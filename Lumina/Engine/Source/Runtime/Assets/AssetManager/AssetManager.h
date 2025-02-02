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

		void Initialize() override;
		void Deinitialize() override;
		
		void LoadAsset(FAssetHandle& Asset);

		
	private:

		FAssetLoadRequest* FindOrCreateRequest(FAssetHandle& Asset);

		void ProcessAssetRequests();
	
	private:

		
		std::thread									AssetRequestThread;
		eastl::atomic<bool>							bAssetThreadRunning = true;
		mutable FRecursiveMutex						RecursiveMutex;

		FFactoryRegistry							FactoryRegistry;
		
		THashMap<FAssetHandle, TWeakPtr<IAsset>>	AssetRecord;

		TQueue<FAssetLoadRequest*>					LoadRequests;
		TSet<FAssetLoadRequest*>					ActiveLoadRequests;

	};
	


	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
