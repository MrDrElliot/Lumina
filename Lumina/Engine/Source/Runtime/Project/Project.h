#pragma once
#include "Containers/String.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{
    class FProject : public TSingleton<FProject>
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
        
        const FSettings& GetProjectSettings() const { return Settings; }


    private:
        
        FSettings   Settings;

        uint8       bHasProjectLoaded:1;
    };
}
