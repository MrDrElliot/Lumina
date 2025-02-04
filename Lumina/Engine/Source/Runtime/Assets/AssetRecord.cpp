#include "AssetRecord.h"

#include "AssetManager/AssetManager.h"
#include "Core/Application/Application.h"


namespace Lumina
{
    void FAssetRecord::AddRef()
    {
        FAssetManager* Manager = FApplication::Get().GetSubsystem<FAssetManager>();
        Assert(Manager);

        FAssetHandle AssetHandle(AssetPath, AssetType);
        Manager->LoadAsset(AssetHandle);
    }

    void FAssetRecord::Release()
    {
        FAssetManager* Manager = FApplication::Get().GetSubsystem<FAssetManager>();
        Assert(Manager);

        FAssetHandle AssetHandle(AssetPath, AssetType);
        Manager->UnloadAsset(AssetHandle);
    }
}
