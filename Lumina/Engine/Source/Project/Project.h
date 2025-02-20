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
    
        
        static TRefCountPtr<Project> New(const char* Name, const std::filesystem::path& Path);
        static TRefCountPtr<Project> Load(const std::filesystem::path& Path);
        static TRefCountPtr<Project> GetCurrent() { return sCurrentProject; }

        void GenerateProjectFromTemplate();
        
        void Serialize();
        void Deserialize(const std::filesystem::path& Path);
        
        static FProjectConfig& GetProjectConfig() { return sCurrentProject->Config; }
        static std::filesystem::path GetProjectContentDirectory() { return sCurrentProject->ProjectDirectory / "Game"; }
        static std::filesystem::path GetProjectRootDirectory() { return sCurrentProject->ProjectDirectory; }


    private:
        
        static TRefCountPtr<Project> sCurrentProject;
        std::filesystem::path ProjectFile;
        std::filesystem::path ProjectDirectory;
        FProjectConfig Config = {};
    
    };
}
