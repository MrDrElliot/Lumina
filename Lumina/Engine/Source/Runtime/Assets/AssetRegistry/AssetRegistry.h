#pragma once

#include <filesystem>

#include "Core/Singleton/Singleton.h"
#include <unordered_map>

#include "Assets/AssetHandle.h"

#define FILE_EXTENSION ".lum"

namespace Lumina
{
	struct FAssetHandle;
	struct FAssetMetadata;

	class AssetRegistry : public TSingleton<AssetRegistry>
	{
	public:

		AssetRegistry();
		~AssetRegistry();

		void StartAssetScan();
		void Shutdown() override;
		
		const FAssetMetadata& GetMetadata(const FAssetHandle& InHandle);
		FAssetMetadata GetMetadataByPath(const std::filesystem::path& InPath);
		void SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata);
		void ScanAssets();

		void GetAllRegisteredAssets(TFastVector<FAssetMetadata>& OutAssets);
		
		bool Exists(const FAssetHandle& InHandle) { return mAssetRegistry.find(InHandle) != mAssetRegistry.end(); }
		uint32 Size() const { return mAssetRegistry.size(); }

		static FAssetHandle ImportAsset(const std::string& Name, void* Data, const std::filesystem::path& ImportFilePath, const std::filesystem::path& NewAssetPath);
		
		template<typename T>
		static TAssetHandle<T> GetAssetByPath(const std::filesystem::path& InPath);
		
	private:

		std::unordered_map<FAssetHandle, FAssetMetadata> mAssetRegistry;
		std::thread ScanThread;
		bool bShouldScan = true;
	};

	template <typename T>
	TAssetHandle<T> AssetRegistry::GetAssetByPath(const std::filesystem::path& InPath)
	{
		AssertMsg(!InPath.empty(), "Empty Asset Path!");
		
		bool bFound = false;
		for (auto& KVP : Get()->mAssetRegistry)
		{
			if(KVP.second.Path == InPath)
			{
				return TAssetHandle<T>(KVP.first);
			}
		}

		FAssetMetadata Metadata;
		FArchive Ar(EArchiverFlags::Reading);
		Ar.ReadFromFile(InPath.string());
		Ar << Metadata;
		Get()->SetMetadata(Metadata.Guid, Metadata);
		return TAssetHandle<T>(Metadata.Guid);
	}
}
