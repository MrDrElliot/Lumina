#include "Project.h"

#include "Log/Log.h"
#include "Platform/Filesystem/FileHelper.h"
#include <nlohmann/json.hpp>
#include <string>

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Engine/Engine.h"
#include "Core/Module/ModuleManager.h"
#include "Core/Object/ObjectBase.h"
#include "Paths/Paths.h"
#include "world/entity/components/EntityComponentRegistry.h"


namespace Lumina
{

    void FProject::LoadProject(const FString& ProjectPath)
    {
        Settings.ProjectName = Paths::FileName(ProjectPath, true);
        Settings.ProjectPath = ProjectPath;
        
        Paths::Mount("project://", GetProjectContentDirectory());

        FString ProjectSolutionPath = Paths::Parent(Paths::RemoveExtension(ProjectPath));
        FString Path = ProjectSolutionPath + "/Binaries/Debug/" + Settings.ProjectName + ".dll";

        if (Paths::Exists(Path))
        {
            if (IModuleInterface* Module = FModuleManager::Get().LoadModule(Path))
            {
                ProcessNewlyLoadedCObjects();
                FEntityComponentRegistry::Get().RegisterAll();
            }
        }
        
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
