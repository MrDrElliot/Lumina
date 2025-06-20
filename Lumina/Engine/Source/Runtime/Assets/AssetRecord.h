#pragma once

#include "Asset.h"
#include "Containers/Array.h"


namespace Lumina
{
    class FAssetManager;
    class FAssetHandle;
}

namespace Lumina
{

    class LUMINA_API FAssetRecord : public IRefCountedObject
    {
    public:

        friend class FAssetRequest;
        friend class FFactory;

        FAssetRecord(const FAssetPath& InPath, EAssetType InType)
            : AssetPath(InPath)
            , AssetType(InType)
        {}

        // IRefCountedObject Interface
        uint32 GetRefCount() const override;
        uint32 AddRef() const override;
        uint32 Release() const override;
        //~ IRefCountedObject Interface

        FORCEINLINE void FreeAssetMemory() { Assert(ReferenceCount == 0); delete AssetPtr; AssetPtr = nullptr; }
        FORCEINLINE void SetLoadingState(EAssetLoadState NewState) { LoadState.store(NewState, eastl::memory_order_relaxed); }
        FORCEINLINE void SetAssetPtr(IAsset* InPtr) { Assert(AssetPtr == nullptr); AssetPtr = InPtr; }

        FORCEINLINE void SetDependencies(TVector<FAssetHandle>&& InDependences) { AssetDependencies = Memory::Move(InDependences); }
        
        FORCEINLINE EAssetLoadState GetLoadState() const    { return LoadState; }
        FORCEINLINE IAsset* GetAssetPtr() const             { return AssetPtr; }
        FORCEINLINE const IAsset* GetConstAssetPtr() const  { return AssetPtr; }
        FORCEINLINE uint32 GetReferenceCount() const        { return ReferenceCount; }
        FORCEINLINE FAssetPath& GetAssetPath()              { return AssetPath; }
        FORCEINLINE EAssetType GetAssetType() const         { return AssetType; }

        FORCEINLINE bool HasLoadingFailed() const { return GetLoadState() == EAssetLoadState::Failed; }
        FORCEINLINE bool IsLoaded() const   { return GetLoadState() == EAssetLoadState::Loaded; }
        FORCEINLINE bool IsUnloaded() const { return GetLoadState() == EAssetLoadState::Unloaded; }

        template<typename T>
        requires(eastl::is_base_of_v<IAsset, T>)
        T* GetAssetPtr()
        {
            return (T*)AssetPtr;
        }
        
    private:

        /** Path this record refers to. */
        FAssetPath                                  AssetPath = {};

        /** Asset type for safety */
        EAssetType                                  AssetType = EAssetType::Max;
        
        /** Raw asset pointer, access is *NOT* thread-safe. */
        mutable IAsset*                             AssetPtr = nullptr;

        /** Assets that this asset may depend on */
        TVector<FAssetHandle>                       AssetDependencies;

        /** State of this resource, thread-safe. If this is loaded, it's safe to assume the AssetPtr is valid memory. */
        mutable eastl::atomic<EAssetLoadState>      LoadState = EAssetLoadState::Unloaded;

        /** Reference count of this resource. */
        mutable uint32                              ReferenceCount = 0;
        
    };
    
}
