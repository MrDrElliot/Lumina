#pragma once

#include "AssetPath.h"
#include "Core/Assertions/Assert.h"
#include "AssetRequester.h"
#include "Containers/Array.h"
#include "GUID/GUID.h"

namespace Lumina
{
    class FAssetRecord
    {
    public:

        friend class FAssetManager;
        
        FAssetRecord(const FGuid& InGuid) : Guid(InGuid)
        {
            Assert(Guid.IsValid());
        }
        
        FORCEINLINE IAsset* GetAssetData() { return Asset; }
        FORCEINLINE const IAsset* GetAssetData() const { return Asset; }
        FORCEINLINE void SetAssetData(IAsset* InData) { Asset = InData; }

        FORCEINLINE void SetLoadState(EAssetLoadState InState) { AssetLoadState = InState; }
        FORCEINLINE EAssetLoadState GetLoadState() const { return AssetLoadState; }

        FORCEINLINE const FGuid& GetAssetGuid() const { return Guid; }

        FORCEINLINE EAssetType GetAssetType() const { return AssetType; }

        FORCEINLINE bool HasReferences() const { return !References.empty(); }
        FORCEINLINE void AddReference(const FAssetRequester& InRequester) { References.emplace_back(InRequester); }
        FORCEINLINE void RemoveReference(const FAssetRequester& InRequester)
        {
            auto Itr = eastl::find(References.begin(), References.end(), InRequester);
            Assert(Itr != References.end());
            References.erase_unsorted(Itr);
        }

        FORCEINLINE void AddDependency(const FGuid& Guid)
        {
            Dependencies.push_back(Guid);
        }
        
        FORCEINLINE bool IsLoading()    const { return AssetLoadState == EAssetLoadState::Loading; }
        FORCEINLINE bool IsLoaded()     const { return AssetLoadState == EAssetLoadState::Loaded; }
        FORCEINLINE bool IsUnloading()  const { return AssetLoadState == EAssetLoadState::Unloading; }
        FORCEINLINE bool IsUnloaded()   const { return AssetLoadState == EAssetLoadState::Unloaded; }

    private:

        FGuid                               Guid;
        EAssetType                          AssetType =         EAssetType::Max;
        IAsset*                             Asset =             nullptr;
        eastl::atomic<EAssetLoadState>      AssetLoadState =    EAssetLoadState::Unloaded;
        TVector<FAssetRequester>            References;       
        TInlineVector<FGuid, 4>             Dependencies;       
        
    };
}
