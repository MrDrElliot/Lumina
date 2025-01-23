#pragma once

#include "Core/Singleton/Singleton.h"
#include <memory>
#include "Assets/Asset.h"
#include "Core/Serialization/Archiver.h"


namespace Lumina
{
	struct FAssetHandle;
	using AssetMap = std::unordered_map<FAssetHandle, std::weak_ptr<LAsset>>;

	class AssetManager : public TSingleton<AssetManager>
	{
	public:

		AssetManager();
		~AssetManager();

		void Shutdown() override
		{
			for (auto& KVP : mAssetMap)
			{
				if(const std::shared_ptr<LAsset>& Asset = KVP.second.lock())
				{
					FArchive Ar(EArchiverFlags::Writing);
					Asset->Serialize(Ar);
					Ar.WriteToFile(Asset->GetAssetMetadata().Path);
				}
			}
		}

		/** Will attempt to return a valid asset, will not automatically load an asset */
		template<typename T>
		std::shared_ptr<T> GetAsset(const FAssetHandle& InHandle)
		{
			if (mAssetMap.find(InHandle) != mAssetMap.end())
			{
				std::weak_ptr<T> WeakPtr = mAssetMap.at(InHandle);
				if (WeakPtr.lock())
				{
					return WeakPtr.lock();
				}
				else
				{
					return std::shared_ptr<T>();
				}
			}
		}

		/** Will attempt to get an asset that is valid, if it is not valid, it will sync load */
		std::shared_ptr<LAsset> LoadSynchronous(const FAssetHandle& InHandle);
		void GetAliveAssets(TFastVector<std::shared_ptr<LAsset>>& OutAliveAssets);

	private:


		AssetMap mAssetMap;


	};
}
