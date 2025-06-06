

#include "AssetRegistry.h"
#include <nlohmann/json.hpp>
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "Project/Project.h"

namespace Lumina
{
    void FAssetRegistry::Initialize(FSubsystemManager& Manager)
    {
        TVector<uint8> Buffer;
        
        FString PathString = FFileHelper::FileFinder("AssetRegistry.json", FProject::Get()->GetProjectRootDirectory());

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
                Header.Guid = FGuid((value["Guid"].get<std::string>().c_str()));
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
            JsonHeader["Guid"] = Reg.second.Guid.String().c_str();
            JsonHeader["Type"] = static_cast<int32>(Reg.second.Type);
            JsonHeader["Path"] =
            {
                {"AssetPath", Reg.second.Path.GetPathAsString().c_str()},
                {"RawPath", Reg.second.Path.GetRawPathAsString().c_str()}
            };
            
            JsonRegistry[Reg.first.GetPathAsString().c_str()] = JsonHeader;
        }

        FString jsonString = JsonRegistry.dump(4).c_str();
        
        FString PathString = FFileHelper::FileFinder("AssetRegistry.json", FProject::Get()->GetProjectRootDirectory());
        
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

    void FAssetRegistry::RenameAsset(FAssetPath& InAsset, const FString& NewName)
    {
        auto Itr = Registry.find(InAsset);
        Assert(Itr != Registry.end());

        FString FinalName = NewName;
        if (!StringUtils::EndsWith(FinalName, ".lasset"))
        {
            FinalName += ".lasset";
        }

        FAssetHeader Header = Registry.at(InAsset);
        Paths::ReplaceFilename(Header.Path.AssetPath, FinalName);
        InAsset.AssetPath = Header.Path.AssetPath;

        Registry.erase(Itr);
        Registry.insert_or_assign(InAsset, Header);
    }
    
    void FAssetRegistry::AssetCreated(const FAssetPath& InPath, const FAssetHeader& Header)
    {
        Registry.insert_or_assign(InPath, Header);
    }

    FAssetHeader FAssetRegistry::FindAssetHeader(const FAssetPath& InPath, bool* bSuccess)
    {
        auto Itr = Registry.find(InPath);

        if (Itr != Registry.end())
        {
            if (bSuccess)
            {
                *bSuccess = true;
            }
            return Itr->second;
        }
        
        if (bSuccess)
        {
            *bSuccess = false;
        }
        
        return {};
    }

    void FAssetRegistry::GetAllAssetHeaders(TVector<FAssetHeader>& Headers)
    {
        for (const auto& Pair : Registry)
        {
            Headers.push_back(Pair.second);
        }
    }
}
