#pragma once

#include "Core/Singleton/Singleton.h"
#include <memory>
#include <EASTL/unordered_map.h>
#include "Assets/Asset.h"
#include "Containers/Array.h"
#include "Core/Serialization/Archiver.h"


namespace Lumina
{
	struct FAssetHandle;
	using AssetMap = eastl::unordered_map<FAssetHandle, eastl::weak_ptr<LAsset>>;

	class AssetManager : public TSingleton<AssetManager>
	{
	public:

		AssetManager();
		~AssetManager();

		void Shutdown() override
		{
			/*for (auto& KVP : mAssetMap)
			{
				if(const TSharedPtr<LAsset>& Asset = KVP.second.lock())
				{
					FArchive Ar(EArchiverFlags::Writing);
					Asset->Serialize(Ar);
					Ar.WriteToFile(Asset->GetAssetMetadata().Path);
				}
			}*/
		}

		/** Will attempt to return a valid asset, will not automatically load an asset */
		template<typename T>
		TSharedPtr<T> GetAsset(const FAssetHandle& InHandle)
		{
			if (mAssetMap.find(InHandle) != mAssetMap.end())
			{
				std::weak_ptr<T> WeakPtr = mAssetMap.at(InHandle);
				if (TSharedPtr<T> AssetPtr = WeakPtr.lock())
				{
					return AssetPtr;
				}
				else
				{
					return TSharedPtr<T>();
				}
			}
			
			return TSharedPtr<T>();
		}

		/** Will attempt to get an asset that is valid, if it is not valid, it will sync load */
		TSharedPtr<LAsset> LoadSynchronous(const FAssetHandle& InHandle);
		void GetAliveAssets(TVector<TSharedPtr<LAsset>>& OutAliveAssets);

	private:


		AssetMap mAssetMap;


	};
}
