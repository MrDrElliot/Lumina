#include "AssetRecord.h"

#include "AssetManager/AssetManager.h"
#include "Core/Application/Application.h"


namespace Lumina
{
    void FAssetRecord::AddRef()
    {
        ReferenceCount++;
    }

    void FAssetRecord::Release()
    {
        ReferenceCount--;
    }
}
