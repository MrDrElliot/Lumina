

#include "AssetRegistry.h"

#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    void FAssetRegistry::Initialize(const FSubsystemManager& Manager)
    {
        TVector<uint8> Buffer;
        using Path = std::filesystem::path;
        
        Path FinalPath = Paths::GetEngineInstallDirectory() / "Lumina" / "Applications" / "LuminaEditor" / "AssetRegistry.lumreg";

        FString PathString = FinalPath.string().c_str();

        if (FFileHelper::LoadFileToArray(Buffer, PathString))
        {
            FMemoryWriter Reader(Buffer);
        
            Serialize(Reader);   
        }
    }

    void FAssetRegistry::Deinitialize()
    {
        TVector<uint8> Buffer;
        FMemoryWriter Writer(Buffer);
        Serialize(Writer);

        using Path = std::filesystem::path;
        
        Path FinalPath = Paths::GetEngineInstallDirectory() / "Lumina" / "Applications" / "LuminaEditor" / "AssetRegistry.lumreg";

        FString PathString = FinalPath.string().c_str();

        FFileHelper::SaveArrayToFile(Buffer, PathString);
    }

    void FAssetRegistry::Serialize(FArchive& Ar)
    {
        Ar << Registry;
    }

    FAssetHeader FAssetRegistry::FindAssetHeader(const FAssetPath& InPath)
    {
        auto Predicate = [InPath] (const FAssetHeader& Value) { return Value.Path == InPath; };

        auto Itr = eastl::find_if(Registry.begin(), Registry.end(), Predicate);

        if (Itr != Registry.end())
        {
            return *Itr;
        }

        return {};
        
    }
}
