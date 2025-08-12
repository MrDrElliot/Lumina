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
		FAssetRequest* LoadAsset(const FString& InAssetPath);

		void NotifyAssetRequestCompleted(FAssetRequest* Request);

		void FlushAsyncLoading();
		
	private:

		FAssetRequest* TryFindActiveRequest(const FString& InAssetPath, bool& bAlreadyInQueue);
		void SubmitAssetRequest(FAssetRequest* Request);
	
	private:
		
		TVector<FAssetRequest*>									ActiveRequests;
		
		std::atomic<int32> OutstandingTasks =					0;
		
	};
	

	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
