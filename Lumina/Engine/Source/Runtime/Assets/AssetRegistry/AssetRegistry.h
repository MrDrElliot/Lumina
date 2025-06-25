#pragma once

#include <filesystem>

#include "Assets/AssetHeader.h"
#include "Core/DisableAllWarnings.h"
#include "Subsystems/Subsystem.h"
#include "Core/Delegates/Delegate.h"
#include "Core/Serialization/MemoryArchiver.h"

PRAGMA_DISABLE_ALL_WARNINGS
#include "EASTL/internal/atomic/atomic.h"
PRAGMA_ENABLE_ALL_WARNINGS

#define FILE_EXTENSION ".lasset"



DECLARE_MULTICAST_DELEGATE(FAssetRegistryUpdatedDelegate);

namespace Lumina
{
	class LUMINA_API FAssetRegistry final : public ISubsystem
	{
	public:

		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void StopDiscoveryThread();
		void UpdateAssetDictionary();
		void UpdateAssetPath(const FName& AssetName, const FString& NewPath);
		FAssetHeader GetAssetHeader(const FName& AssetName);
		
	protected:
		
		void BeginAssetInitialDiscovery();
		void BuildAssetDictionary(const TVector<FString>& DiscoveredAssets);

		std::thread							DiscoveryThread;
		eastl::atomic<bool> 				bDiscoveryThreadRunning;
		THashMap<FName, FAssetHeader> 		AssetDictionary;
	};
}
