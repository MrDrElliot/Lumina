

#include "AssetRegistry.h"
#include <nlohmann/json.hpp>
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
        TVector<uint8> Buffer;
        using Path = std::filesystem::path;
        
        Path FinalPath = Paths::GetEngineInstallDirectory() / "Applications" / "LuminaEditor" / "AssetRegistry.json";
        FString PathString = FinalPath.string().c_str();

        FString FileString;
        if (!FFileHelper::LoadFileIntoString(FileString, PathString))
        {
            return;
        }
        
        LoadRegistry(FileString);
        
    }

    void FAssetRegistry::Deinitialize()
    {
        SaveRegistry();
    }

    void FAssetRegistry::LoadRegistry(const FString& File)
    {
        using namespace nlohmann;

        try
        {
            json JsonRegistry = json::parse(File.c_str());
            Registry.clear();

            for (auto& [key, value] : JsonRegistry.items())
            {
                FAssetPath Path(value["Path"]["AssetPath"].get<std::string>().c_str(), value["Path"]["RawPath"].get<std::string>().c_str());
                
                FAssetHeader Header;
                Header.Version = value["Version"].get<int32>();
                Header.Guid.FromString(value["Guid"].get<std::string>().c_str());
                Header.Type = static_cast<EAssetType>(value["Type"].get<int32>());
                Header.Path = Path;

                for (const auto& Dep : value["Dependencies"])
                {
                    //Header.Dependencies.push_back(FAssetHandle(Dep["AssetPath"]<std::string>().c_str(), static_cast<EAssetType>(Dep.get<int32>())));
                }

                Registry[Path] = Header;
            }
        }
        catch (const json::exception& e)
        {
            LOG_ERROR("JSON Parsing error: {0}", e.what());
        }
    }


    void FAssetRegistry::SaveRegistry()
    {
        using namespace nlohmann;

        json JsonRegistry;

        for (const auto& Reg : Registry)
        {
            json JsonHeader;
            JsonHeader["Version"] = Reg.second.Version;
            JsonHeader["Guid"] = Reg.second.Guid.ToString().c_str();
            JsonHeader["Type"] = static_cast<int32>(Reg.second.Type);
            JsonHeader["Path"] =
            {
                {"AssetPath", Reg.second.Path.GetPathAsString().c_str()},
                {"RawPath", Reg.second.Path.GetRawPathAsString().c_str()}
            };

            // Handle Dependencies
            json JsonDependencies = json::array();
            for (const auto& Dep : Reg.second.Dependencies)
            {
                JsonDependencies.push_back(Dep.AssetPath.GetPathAsString().c_str());
            }
            
            JsonHeader["Dependencies"] = JsonDependencies;

            JsonRegistry[Reg.first.GetPathAsString().c_str()] = JsonHeader;
        }

        FString jsonString = JsonRegistry.dump(4).c_str();

        std::filesystem::path FinalPath = Paths::GetEngineInstallDirectory() / "Applications" / "LuminaEditor" / "AssetRegistry.json";
        FString PathString = FinalPath.string().c_str();
        
        FFileHelper::SaveStringToFile(jsonString, PathString);
    }


    void FAssetRegistry::CreateAssetHeader(const FAssetPath& InPath)
    {

        TVector<uint8> Buffer;
        FFileHelper::LoadFileToArray(Buffer, InPath.GetPathAsString());
        
        FMemoryReader Reader(Buffer);

        FAssetHeader Header;
        Reader << Header;

        Assert(Header.Path == InPath.GetPathAsString());

        Registry.insert_or_assign(InPath, Header);

        SaveRegistry();
    }

    FAssetHeader FAssetRegistry::FindAssetHeader(const FAssetPath& InPath)
    {
        auto Itr = Registry.find(InPath);

        if (Itr != Registry.end())
        {
            return Itr->second;
        }

        return {};
    }
}
