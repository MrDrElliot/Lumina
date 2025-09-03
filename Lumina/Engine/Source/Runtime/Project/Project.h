#pragma once
#include "Containers/String.h"
#include "Core/Singleton/Singleton.h"
#include "Module/API.h"

namespace Lumina
{
    class FProject : public TSingleton<FProject>
    {
    public:

        FProject()
            : bHasProjectLoaded(false)
        {}
        
        LUMINA_API struct FSettings
        {
            FString ProjectName;
            FString ProjectPath;
        };

        LUMINA_API void LoadProject(const FString& ProjectPath);
        
        LUMINA_API bool HasLoadedProject() const { return bHasProjectLoaded; }
        LUMINA_API const FString& GetProjectPath() const { return Settings.ProjectPath; }
        LUMINA_API FString GetProjectRootDirectory() const;
        
        LUMINA_API const FSettings& GetProjectSettings() const { return Settings; }

        LUMINA_API FString GetProjectContentDirectory() const;
    

    private:
        
        FSettings   Settings;

        uint8       bHasProjectLoaded:1;
    };
}
