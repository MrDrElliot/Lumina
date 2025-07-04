#pragma once
#include "Containers/String.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    class LUMINA_API FProject : public TSingleton<FProject>
    {
    public:

        FProject()
            : Settings()
            , bHasProjectLoaded(false)
        {}
        
        struct FSettings
        {
            FString ProjectName;
            FString ProjectPath;
        };

        void LoadProject(const FString& ProjectPath);
        

        FORCEINLINE bool HasLoadedProject() const { return bHasProjectLoaded; }
        const FString& GetProjectPath() const { return Settings.ProjectPath; }
        FString GetProjectRootDirectory() const;
        
        const FSettings& GetProjectSettings() const { return Settings; }

        FString GetProjectContentDirectory() const;
    

    private:
        
        FSettings   Settings;

        uint8       bHasProjectLoaded:1;
    };
}
