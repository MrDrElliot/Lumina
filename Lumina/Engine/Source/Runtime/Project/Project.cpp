﻿#include "Project.h"

#include "Log/Log.h"
#include "Platform/Filesystem/FileHelper.h"
#include <nlohmann/json.hpp>
#include <string>

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Engine/Engine.h"
#include "Paths/Paths.h"


namespace Lumina
{

    void FProject::LoadProject(const FString& ProjectPath)
    {
        FString ProjectJson;
        if (!FileHelper::LoadFileIntoString(ProjectJson, ProjectPath))
        {
            LOG_ERROR("Failed to load project at location: {0}", ProjectPath);
            return;
        }
        
        Settings.ProjectPath = ProjectPath;
        
        try
        {
            nlohmann::json JsonData = nlohmann::json::parse(ProjectJson.c_str());

            if (JsonData.contains("ProjectName"))
            {
                Settings.ProjectName = JsonData["ProjectName"].get<std::string>().c_str();
            }

            LOG_INFO("Loaded project: {0} from {1}", Settings.ProjectName.c_str(), Settings.ProjectPath.c_str());
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse project JSON: {0}", e.what());
        }

        Paths::Mount("project://", GetProjectContentDirectory());
        GEngine->GetEngineSubsystem<FAssetRegistry>()->ProjectLoaded();
        bHasProjectLoaded = true;
    }

    FString FProject::GetProjectRootDirectory() const
    {
        std::filesystem::path Path = GetProjectSettings().ProjectPath.c_str();
        Path = Path.parent_path();

        return FString(Path.string().c_str());
    }

    FString FProject::GetProjectContentDirectory() const
    {
        std::filesystem::path Path = GetProjectSettings().ProjectPath.c_str();
        Path = Path.parent_path() / "Game" / "Content";
        FString StringPath(Path.string().c_str());

        StringUtils::ReplaceAllOccurrencesInPlace(StringPath, "\\", "/");
        
        return StringPath;
    }
}
