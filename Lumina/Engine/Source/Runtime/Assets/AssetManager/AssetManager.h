#pragma once

#include "Core/Functional/Function.h"
#include "Assets/Asset.h"
#include "Assets/AssetRecord.h"
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

		struct FPendingRequest
		{
			enum class Type { Load, Unload };

		public:

			FPendingRequest() = default;

			FPendingRequest(Type InType, FAssetRecord* InRecord, const FAssetRequester& InRequesterID)
				: Record(InRecord)
				, RequesterID(InRequesterID)
				, Type(InType)
			{
				Assert(InRecord);
			}

			FAssetRecord*			Record = nullptr;
			FAssetRequester			RequesterID;
			Type                    Type = Type::Load;
		};

		FAssetManager();
		~FAssetManager() override;

		void Initialize() override;
		void Deinitialize() override;

		void Update(bool bWaitForAsyncTasks = false);

		
		void LoadAsset(FAssetHandle& InHandle, const FAssetRequester& Requester = FAssetRequester());

		void UnloadAsset(FAssetHandle& InHandle, const FAssetRequester& Requester = FAssetRequester());

	
	private:
		
		ELoadResult LoadFromDisk(const FAssetHandle& InAssetHandle, const FAssetPath& InPath, FAssetRecord* InRecord);

		FAssetRecord* FindOrCreateAssetRecord(const FGuid& InGuid);
		FAssetRecord* FindAssetRecordChecked(const FGuid& InGuid);
		
		void AddPendingRequest(FPendingRequest&& NewRequest);

		FAssetRequest* TryFindActiveRequest(const FAssetRecord* InRecord) const;
		
		void ProcessAssetRequests();
		
	private:

		
		std::thread									AssetRequestThread;
		eastl::atomic<bool>							bAssetThreadRunning = true;
		mutable FRecursiveMutex						RecursiveMutex;

		FFactoryRegistry							FactoryRegistry;
		
		THashMap<FGuid, FAssetRecord*>				AssetRecord;
		
		TVector<FPendingRequest>					PendingRequests;
		TVector<FAssetRequest*>						ActiveRequests;
		TVector<FAssetRequest*>						CompletedRequests;

	};
	


	//--------------------------------------------------------------------------
	// Template definitions.
	//--------------------------------------------------------------------------
	
}
