#include "Project.h"

#include <filesystem>
#include <fstream>
#include <string>

#include "GUID/GUID.h"
#include "yaml-cpp/yaml.h"

namespace Lumina
{

    TRefPtr<Project> Project::sCurrentProject;
    
    TRefPtr<Project> Project::New(const char* Name, const std::filesystem::path& Path)
    {
        std::filesystem::path ProjectFileName = std::filesystem::path(Name).concat(".lproject");
        std::filesystem::path FullProjectPath = Path / Name / ProjectFileName;
        
        sCurrentProject = MakeRefPtr<Project>();
        sCurrentProject->ProjectFile = FullProjectPath;
        sCurrentProject->ProjectDirectory = Path / Name;
        sCurrentProject->Config.Name = Name;
        sCurrentProject->Config.EngineVersion = "NULL";
        sCurrentProject->GenerateProjectFromTemplate();
        sCurrentProject->Serialize();
        return sCurrentProject;
    }

    TRefPtr<Project> Project::Load(const std::filesystem::path& Path)
    {
        sCurrentProject = MakeRefPtr<Project>();
        sCurrentProject->ProjectFile = Path;
        sCurrentProject->ProjectDirectory = Path.parent_path();
        sCurrentProject->Deserialize(Path);
        return sCurrentProject;
    }
    

    void Project::GenerateProjectFromTemplate()
    {
        std::filesystem::path targetDir = ProjectDirectory;
        try
        {
            std::filesystem::create_directories(targetDir / "Game" / "Content");
            std::filesystem::create_directory(targetDir / "Game" / "Scripts");
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR("Error while generating project from template: {0}", ex.what());
        }
    }

    void Project::Serialize()
    {
        YAML::Emitter data;
        data << YAML::BeginMap;
        data << YAML::Key << "Project" << YAML::Value;
        {
            data << YAML::BeginMap;
            data << YAML::Key << "Name" << Config.Name;
            data << YAML::Key << "ProjectVersion" << Config.ProjectVersion;
            data << YAML::Key << "EngineVersion" << Config.EngineVersion;
            data << YAML::Key << "StartupScene" << Config.StartupScene;
        }
        data << YAML::EndMap;

        // Write to file
        std::ofstream stream(ProjectFile);
        if (stream.is_open())
        {
            stream << data.c_str();
            stream.close();
        }
        else
        {
            LOG_ERROR("Failed to open file for serialization: {0}", ProjectFile.string());
        }
    }

    
    void Project::Deserialize(const std::filesystem::path& path)
    {
        std::ifstream stream(path);
        if (stream.is_open())
        {
            std::stringstream strStream;
            strStream << stream.rdbuf();

            YAML::Node data = YAML::Load(strStream.str());
            if(!data["Project"])
            {
                return;
            }
            YAML::Node rootNode = data["Project"];
            
            Config.Name =           rootNode["Name"].as<std::string>();
            Config.ProjectVersion = rootNode["ProjectVersion"].as<std::string>();
            Config.EngineVersion =  rootNode["EngineVersion"].as<std::string>();
        
            stream.close();
        }
        else
        {
            LOG_ERROR("Failed to open file for deserialization: {0}", path.string());
        }
    }


}
