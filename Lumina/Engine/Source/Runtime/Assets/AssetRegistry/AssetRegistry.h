#pragma once

#include <filesystem>

#include "AssetData.h"
#include "Assets/AssetHeader.h"
#include "Core/DisableAllWarnings.h"
#include "Subsystems/Subsystem.h"
#include "Core/Delegates/Delegate.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Threading/Thread.h"

PRAGMA_DISABLE_ALL_WARNINGS
#include "EASTL/internal/atomic/atomic.h"
PRAGMA_ENABLE_ALL_WARNINGS

#define FILE_EXTENSION ".lasset"



DECLARE_MULTICAST_DELEGATE(FAssetRegistryUpdatedDelegate);

namespace Lumina
{
	class CPackage;
	struct FAssetData;
	class CClass;
	
	struct FARFilter
	{
		TVector<FString> Paths;
		TVector<FString> ClassNames;
	};

	struct FAssetDataPtrHash
	{
		size_t operator() (const FAssetData* Asset) const noexcept
		{
			size_t Seed;
			Hash::HashCombine(Seed, Asset->FullPath);
			
			return Seed;
		}
	};

	struct FAssetDataPtrEqual
	{
		bool operator()(const FAssetData* A, const FAssetData* B) const noexcept
		{
			return A->FullPath == B->FullPath;
		}
	};


	using FAssetDataMap = THashSet<FAssetData*, FAssetDataPtrHash, FAssetDataPtrEqual>;
	
	
	class LUMINA_API FAssetRegistry final : public ISubsystem
	{
	public:

		void ProjectLoaded();
		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void RunInitialDiscovery();
		void OnInitialDiscoveryCompleted();

		void AssetCreated(CObject* Asset);
		void AssetDeleted(CPackage* Package);
		void AssetRenamed(CObject* Asset, const FString& OldPackagePath);
		void AssetSaved(CPackage* Package);

		FAssetRegistryUpdatedDelegate& GetOnAssetRegistryUpdated() { return OnAssetRegistryUpdated; }

		const THashMap<FName, FAssetData*>& GetAssetPackageMap() const { return AssetPackageMap; }
		const THashMap<FName, TVector<FAssetData*>>& GetAssetsByPath() const { return AssetsByPath; }
		const TVector<FAssetData*>& GetAssetsForPath(const FName& Path);
		const FAssetDataMap& GetAssets() const { return Assets; }
		
	private:

		void ProcessPackagePath(const FString& Path);
		
		void ClearAssets();

		void BroadcastRegistryUpdate();


		FAssetRegistryUpdatedDelegate	OnAssetRegistryUpdated;
		
		FMutex							AssetsMutex;

		/** Global hash of all registered assets */
		FAssetDataMap 					Assets;

		/** Package names to asset data */
		THashMap<FName, FAssetData*>	AssetPackageMap;

		/** Map if full names to assets saved on disk */
		THashMap<FName, TVector<FAssetData*>> AssetsByPath;
	};
}
