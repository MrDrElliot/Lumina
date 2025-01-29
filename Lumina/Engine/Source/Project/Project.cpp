#include "Project.h"

#include <filesystem>
#include <fstream>
#include "Containers/String.h"
#include "GUID/GUID.h"
#include "Paths/Paths.h"
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
        std::filesystem::path FullPath = Paths::ResolveFromEngine(Path);
        sCurrentProject = MakeRefPtr<Project>();
        sCurrentProject->ProjectFile = FullPath;
        sCurrentProject->ProjectDirectory = FullPath.parent_path();
        sCurrentProject->Deserialize(FullPath);
        return sCurrentProject;
    }
    

    void Project::GenerateProjectFromTemplate()
    {
        std::filesystem::path targetDir = std::filesystem::absolute(ProjectDirectory);
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
            data << YAML::Key << "Name" << Config.Name.c_str();
            data << YAML::Key << "ProjectVersion" << Config.ProjectVersion.c_str();
            data << YAML::Key << "EngineVersion" << Config.EngineVersion.c_str();
            data << YAML::Key << "StartupScene" << Config.StartupScene.c_str();
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
            
            Config.Name =           rootNode["Name"].as<std::string>().c_str();
            Config.ProjectVersion = rootNode["ProjectVersion"].as<std::string>().c_str();
            Config.EngineVersion =  rootNode["EngineVersion"].as<std::string>().c_str();
        
            stream.close();
        }
        else
        {
            LOG_ERROR("Failed to open file for deserialization: {0}", path.string());
        }
    }


}
