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
        inline std::filesystem::path GetStartupProject() const { return StartupProject; }
        void SetStartupProject(const std::filesystem::path& Path) { StartupProject = Path; }

        void Shutdown() override {}

        FORCEINLINE void Serialize()
        {
            YAML::Emitter Emitter;
            Emitter << YAML::BeginMap;
            Emitter << YAML::Key << "Editor Settings" << YAML::Value;
            {
                Emitter << YAML::BeginMap;
                Emitter << YAML::Key << "Startup Project" << YAML::Value << StartupProject.string();
                Emitter << YAML::EndMap;
            }
            Emitter << YAML::EndMap;

            std::filesystem::path filePath = Paths::ResolveFromEngine("Applications/LuminaEditor/Config/EditorSettings.yaml");

            std::filesystem::create_directories(filePath.parent_path());
            std::ofstream stream(filePath);
            if (stream.is_open())
            {
                stream << Emitter.c_str();
                stream.close();
            }
            else
            {
                LOG_ERROR("Failed to open Editor Settings for writing: {}", filePath.string());
            }
        }

        FORCEINLINE void Deserialize()
        {
            std::filesystem::path configPath = Paths::ResolveFromEngine("Applications/LuminaEditor/Config/EditorSettings.yaml");

            std::ifstream stream(configPath);
            if (stream.is_open())
            {
                std::stringstream strStream;
                strStream << stream.rdbuf();

                YAML::Node data = YAML::Load(strStream.str());
                if (!data["Editor Settings"])
                {
                    return;
                }

                YAML::Node rootNode = data["Editor Settings"];
                StartupProject = rootNode["Startup Project"].as<std::string>();

                if (StartupProject.empty())
                {
                    StartupProject = "Applications/Sandbox/Sandbox.lproject";
                    Serialize();
                }

                stream.close();
            }
            else
            {
                LOG_ERROR("Failed to open Editor Settings: {}", configPath.string());

                // Ensure the file exists, then reattempt deserialization
                Serialize();
                Deserialize();
            }
        }

    private:
        std::filesystem::path StartupProject;
    };
}
