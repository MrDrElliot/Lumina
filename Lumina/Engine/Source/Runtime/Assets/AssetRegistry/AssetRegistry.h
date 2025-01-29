#pragma once

#include <filesystem>

#include "Core/Singleton/Singleton.h"
#include <unordered_map>
#include <any>

#include "Assets/AssetHandle.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"

#define FILE_EXTENSION ".lum"

#define TEXTURE_ASSET_VERSION 1;
#define STATICMESH_ASSET_VERSION 1;

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
		
		FAssetMetadata GetMetadata(const FAssetHandle& InHandle);
		FAssetMetadata GetMetadataByPath(const std::filesystem::path& InPath);
		void SetMetadata(const FAssetHandle& InHandle, const FAssetMetadata& InMetadata);
		void ScanAssets();

		void GetAllRegisteredAssets(TVector<FAssetMetadata>& OutAssets);
		void GetAllAssetsOfType(EAssetType Type, TVector<FAssetMetadata>& OutAssets);
		
		bool Exists(const FAssetHandle& InHandle) { return mAssetRegistry.find(InHandle) != mAssetRegistry.end(); }
		uint32 Size() const { return mAssetRegistry.size(); }

		static FAssetHandle ImportAsset(const FString& Name, void* Data, const std::filesystem::path& ImportFilePath, const std::filesystem::path& NewAssetPath);
		static FAssetHandle CreateAsset(EAssetType Type, const FString& Name, void* Data, const std::filesystem::path& NewAssetPath);
		
		template<typename T>
		static TAssetHandle<T> GetAssetByPath(const std::filesystem::path& InPath);
		
	private:

		eastl::unordered_map<FAssetHandle, FAssetMetadata> mAssetRegistry;
		eastl::unordered_map<EAssetType, TVector<FAssetMetadata>> AssetTypeMap;

		std::thread ScanThread;
		std::atomic<bool> bShouldScan = true;
	};

	template <typename T>
	TAssetHandle<T> AssetRegistry::GetAssetByPath(const std::filesystem::path& InPath)
	{
		AssertMsg(!InPath.empty(), "Empty Asset Path!");
		
		for (auto& KVP : Get()->mAssetRegistry)
		{
			if(KVP.second.Path == InPath)
			{
				return TAssetHandle<T>(KVP.first);
			}
		}

		FAssetMetadata Metadata;
		TVector<uint8> Buffer;
		FMemoryReader Reader(Buffer);
		Reader << Metadata;
		
		if (FFileHelper::LoadFileToArray(Buffer, InPath))
		{
			LOG_ERROR("Failed to load file for asset: {0}", InPath.string());
			return;
		}
		
		Get()->SetMetadata(Metadata.Guid, Metadata);
		return TAssetHandle<T>(Metadata.Guid);
	}
}
