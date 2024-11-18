#pragma once
#include <filesystem>
#include <string>

#include "Memory/RefCounted.h"

namespace Lumina
{

    class Project : public RefCounted
    {
    public:
        
        struct FProjectConfig
        {
            std::string Name;
            std::string ProjectVersion;
            std::string EngineVersion;
            std::string StartupScene;
        };
    
        
        static TRefPtr<Project> New(const char* Name, const std::filesystem::path& Path);
        static TRefPtr<Project> Load(const std::filesystem::path& Path);
        static TRefPtr<Project> GetCurrent() { return sCurrentProject; }

        void GenerateProjectFromTemplate();
        
        void Serialize();
        void Deserialize(const std::filesystem::path& Path);
        
        static FProjectConfig& GetProjectConfig() { return sCurrentProject->Config; }
        static std::filesystem::path GetProjectContentDirectory() { return sCurrentProject->ProjectDirectory / "Game"; }

    private:
        
        static TRefPtr<Project> sCurrentProject;
        std::filesystem::path ProjectFile;
        std::filesystem::path ProjectDirectory;
        FProjectConfig Config = {};
    
    };
}
