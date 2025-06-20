#include "AssetRecord.h"

#include "AssetManager/AssetManager.h"


namespace Lumina
{
    uint32 FAssetRecord::GetRefCount() const
    {
        return ReferenceCount;
    }

    uint32 FAssetRecord::AddRef() const
    {
        ReferenceCount++;
        return uint32(ReferenceCount);

    }

    uint32 FAssetRecord::Release() const
    {
        ReferenceCount--;

        // A reference count of 1 means the asset manager is the only remaining reference to the asset, so we can safely delete it.
        if (ReferenceCount == 1)
        {
            Memory::Delete(AssetPtr);
            LoadState = EAssetLoadState::Unloaded;
            AssetPtr = nullptr;
        }

        // The asset record iself is officially done, so we can delete it here.
        else if (ReferenceCount == 0)
        {
            FAssetRecord* MutableThis = const_cast<FAssetRecord*>(this);
            Memory::Delete(MutableThis);
        }

        return uint32(ReferenceCount);
    }
}
