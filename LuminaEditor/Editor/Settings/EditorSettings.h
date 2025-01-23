#pragma once
#include <filesystem>
#include <fstream>

#include "Core/Singleton/Singleton.h"
#include "Log/Log.h"
#include "Paths/Paths.h"
#include "yaml-cpp/yaml.h"

namespace Lumina
{
    class FEditorSettings : public TSingleton<FEditorSettings>
    {
    public:

        // Will only be valid after initialize deserialization.
        inline std::filesystem::path GetStartupProject() const { return StartupProject; }
        void SetStartupProject(const std::filesystem::path& Path) { StartupProject = Path.string();}
        
        void Shutdown() override {}
        
        void Serialize()
        {
            YAML::Emitter Emitter;
            Emitter << YAML::BeginMap;
            Emitter << YAML::Key << "Editor Settings" << YAML::Value;
            {
                Emitter << YAML::BeginMap;
                Emitter << YAML::Key << "Startup Project" << StartupProject;
            }
            Emitter << YAML::EndMap;

            // Write to file
            // Define the file path
            std::filesystem::path filePath = Paths::GetEngineInstallDirectory() / "LuminaEditor" / "Config" / "EditorSettings.yaml";
    
            // Create the directory if it doesn't exist
             std::filesystem::create_directories(filePath.parent_path());
            std::ofstream stream(filePath);
            if (stream.is_open())
            {
                stream << Emitter.c_str();
                stream.close();
            }
            else
            {
                LOG_ERROR("Failed to open Editor Settings");
            }
        }
        void Deserialize()
        {
            std::ifstream stream(Paths::GetEngineInstallDirectory() / "LuminaEditor" / "Config" / "EditorSettings.yaml");
            if (stream.is_open())
            {
                std::stringstream strStream;
                strStream << stream.rdbuf();

                YAML::Node data = YAML::Load(strStream.str());
                if(!data["Editor Settings"])
                {
                    return;
                }
                
                YAML::Node rootNode = data["Editor Settings"];
            
                StartupProject = rootNode["Startup Project"].as<std::string>();
                
                if(StartupProject.empty())
                {
                    StartupProject = std::filesystem::path(Paths::GetEngineInstallDirectory() / "Sandbox" / "Sandbox.lproject").string();
                    stream.close();

                    Serialize();
                }
                
                stream.close();
            }
            else if(!std::filesystem::exists(Paths::GetEngineInstallDirectory() / "LuminaEditor" / "Config" / "EditorSettings.yaml"))
            {
                Serialize();
                Deserialize();
                LOG_ERROR("Failed to open Editor Settings");
            }
        }

    private:

        std::string StartupProject;
    };
}
