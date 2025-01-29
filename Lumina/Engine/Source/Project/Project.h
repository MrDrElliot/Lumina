#pragma once
#include <filesystem>
#include <string>

#include "Memory/RefCounted.h"

namespace Lumina
{

    class Project : public FRefCounted
    {
    public:
        
        struct FProjectConfig
        {
            FString Name;
            FString ProjectVersion;
            FString EngineVersion;
            FString StartupScene;
        };
    
        
        static TRefPtr<Project> New(const char* Name, const std::filesystem::path& Path);
        static TRefPtr<Project> Load(const std::filesystem::path& Path);
        static TRefPtr<Project> GetCurrent() { return sCurrentProject; }

        void GenerateProjectFromTemplate();
        
        void Serialize();
        void Deserialize(const std::filesystem::path& Path);
        
        static FProjectConfig& GetProjectConfig() { return sCurrentProject->Config; }
        static std::filesystem::path GetProjectContentDirectory() { return sCurrentProject->ProjectDirectory / "Game"; }
        static std::filesystem::path GetProjectRootDirectory() { return sCurrentProject->ProjectDirectory; }


    private:
        
        static TRefPtr<Project> sCurrentProject;
        std::filesystem::path ProjectFile;
        std::filesystem::path ProjectDirectory;
        FProjectConfig Config = {};
    
    };
}
