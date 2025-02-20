#pragma once

#include "Asset.h"
#include "Containers/Array.h"


namespace Lumina
{
    class FAssetManager;
}

namespace Lumina
{
    class FAssetHandle;

    class FAssetRecord
    {
    public:

        friend class FAssetRequest;
        friend class FFactory;

        FAssetRecord(FAssetManager* InManager, const FAssetPath& InPath, EAssetType InType)
            : Manager(InManager)
            , AssetPath(InPath)
            , AssetType(InType)
        {}

        void AddRef();
        void Release();

        FORCEINLINE void FreeAssetMemory() { Assert(ReferenceCount == 0); delete AssetPtr; AssetPtr = nullptr; }
        FORCEINLINE void SetLoadingState(EAssetLoadState NewState) { LoadState = NewState; }
        FORCEINLINE void SetAssetPtr(IAsset* InPtr) { Assert(AssetPtr == nullptr); AssetPtr = InPtr; }

        FORCEINLINE void SetDependencies(TVector<FAssetHandle>&& InDependences) { AssetDependencies = FMemory::Move(InDependences); }
        
        FORCEINLINE EAssetLoadState GetLoadState() const    { return LoadState; }
        FORCEINLINE IAsset* GetAssetPtr() const             { return AssetPtr; }
        FORCEINLINE const IAsset* GetConstAssetPtr() const  { return AssetPtr; }
        FORCEINLINE uint32 GetReferenceCount() const        { return ReferenceCount; }
        FORCEINLINE FAssetPath& GetAssetPath()              { return AssetPath; }
        FORCEINLINE EAssetType GetAssetType() const         { return AssetType; }

        FORCEINLINE bool IsLoaded() const   { return GetLoadState() == EAssetLoadState::Loaded; }
        FORCEINLINE bool IsUnloaded() const { return GetLoadState() == EAssetLoadState::Unloaded; }
        
    private:

        /** Path this record refers to. */
        FAssetPath                          AssetPath = {};

        /** Asset type for safety */
        EAssetType                          AssetType = EAssetType::Max;
        
        /** Raw asset pointer, access is *NOT* thread-safe. */
        IAsset*                             AssetPtr = nullptr;

        /** Assets that this asset may depend on */
        TVector<FAssetHandle>               AssetDependencies;

        /** State of this resource, thread-safe. If this is loaded, it's safe to assume the AssetPtr is valid memory. */
        eastl::atomic<EAssetLoadState>      LoadState = EAssetLoadState::Unloaded;

        /** Reference count of this resource. */
        uint32                              ReferenceCount = 0;

        /** Reference to asset manager which keeps this record, acess is circular and we need a better solution */
        FAssetManager*                      Manager = nullptr;
    };
    
}
