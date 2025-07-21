#pragma once

#include <filesystem>

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
	class CClass;

	struct FAssetData
	{
		FName	Name;
		FString	ClassName;
		FString Path;
	};

	struct FARFilter
	{
		TVector<FString> Paths;
		TVector<FString> ClassNames;
	};
	
	class LUMINA_API FAssetRegistry final : public ISubsystem
	{
	public:

		void ProjectLoaded();
		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void BuildAssetDictionary();

		void GetAssets(const FARFilter& Filter, TVector<FAssetData>& OutAssets);
		FAssetData GetAsset(const FString& Path);

		bool IsPathCorrupt(const FString& Path) { return CorruptedAssets.find(FName(Path)) != CorruptedAssets.end(); }

		
	private:
		

		void AddAsset(const FAssetData& Data);
		void ClearAssets();

		TSet<FName>					CorruptedAssets;
		
		std::condition_variable		BuildCV;
		bool bIsBuildingAssets =	false;
		bool bBuildQueued =			false;

		FMutex						BuildMutex;
		FMutex						AssetsMutex;
		TVector<FAssetData> 		Assets;
		THashMap<FName, FAssetData> AssetPathMap;
		FThread						ScanThread;
	};
}
