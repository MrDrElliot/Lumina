#pragma once

#include <filesystem>
#include "Subsystems/Subsystem.h"
#include "Assets/AssetHandle.h"
#include "Core/Delegates/Delegate.h"
#include "Core/Serialization/MemoryArchiver.h"

#define FILE_EXTENSION ".lum"


DECLARE_MULTICAST_DELEGATE(FAssetRegistryUpdatedDelegate);

namespace Lumina
{
	struct FAssetHeader;

	class FAssetRegistry : public ISubsystem
	{
	public:

		FAssetRegistry() = default;
		~FAssetRegistry() override = default;

		void Initialize(FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void LoadRegistry(const FString& File);
		void SaveRegistry();
		
		void CreateAssetHeader(const FAssetPath& InPath);

		void AssetCreated(const FAssetPath& InPath, const FAssetHeader& Header);
		
		FAssetHeader FindAssetHeader(const FAssetPath& InPath);
	
	private:
		
		THashMap<FAssetPath, FAssetHeader>			Registry;
		FAssetRegistryUpdatedDelegate				OnAssetRegistryUpdated;
	};
}
