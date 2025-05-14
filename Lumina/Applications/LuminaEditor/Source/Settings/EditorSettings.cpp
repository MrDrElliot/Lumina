#include "EditorSettings.h"

#include <nlohmann/json.hpp>

#include "Core/Assertions/Assert.h"
#include "Paths/Paths.h"

namespace Lumina
{
    void FEditorSettings::LoadSettings()
    {
        std::filesystem::path ConfigPath = Paths::ResolveFromEngine("Applications/LuminaEditor/Config/EditorSettings.json");

        if (!FFileHelper::DoesFileExist(ConfigPath.string().c_str()))
        {
            LOG_INFO("Could not find EditorSettings.json, creating default");
            
            nlohmann::json DefaultJson;
            DefaultJson["StartupProject"] = "NULL";

            FString DefaultJsonString = DefaultJson.dump(4).c_str();
            FFileHelper::SaveStringToFile(DefaultJsonString, ConfigPath.string().c_str());
        }
        
        FString FileString;
        if (!FFileHelper::LoadFileIntoString(FileString, ConfigPath.string().c_str()))
        {
            LOG_ERROR("Failed to load EditorSettings.json");
            return;
        }

        try
        {
            nlohmann::json JsonData = nlohmann::json::parse(FileString.c_str());

            if (JsonData.contains("StartupProject") && JsonData["StartupProject"].is_string())
            {
                StartupProject = JsonData["StartupProject"].get<std::string>().c_str();
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Error parsing EditorSettings.json: {0}", e.what());
        }
        
    }

    void FEditorSettings::SaveSettings()
    {
        std::filesystem::path ConfigPath = Paths::ResolveFromEngine("Applications/LuminaEditor/Config/EditorSettings.json");

        nlohmann::json JsonData;
        JsonData["StartupProject"] = StartupProject.c_str();

        FString JsonString = JsonData.dump(4).c_str();
        FFileHelper::SaveStringToFile(JsonString, ConfigPath.string().c_str());
    }
}
