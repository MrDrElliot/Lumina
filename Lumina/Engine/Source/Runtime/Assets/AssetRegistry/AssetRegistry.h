#pragma once

#include <filesystem>
#include <unordered_map>
#include <any>
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

		void Initialize(const FSubsystemManager& Manager) override;
		void Deinitialize() override;

		void Serialize(FArchive& Ar);

		FAssetHeader FindAssetHeader(const FAssetPath& InPath);
		
		TVector<FAssetHeader>& GetAssets() { return Registry; }
		
	private:
		
		TVector<FAssetHeader>						Registry;
		FAssetRegistryUpdatedDelegate				OnAssetRegistryUpdated;
	};
}
