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

		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void BuildAssetDictionary();

		void GetAssets(const FARFilter& Filter, TVector<FAssetData>& OutAssets);
		FAssetData GetAsset(const FString& Path);

	private:

		FMutex						Mutex;
		TVector<FAssetData> 		Assets;
		THashMap<FName, FAssetData> AssetPathMap;
	};
}
