#pragma once

#include "Core/Singleton/Singleton.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    class FEditorSettings : public TSingleton<FEditorSettings>
    {
    public:
        
        const FString& GetStartupProject() const { return StartupProject; }
        void SetStartupProject(const FString& Path) { StartupProject = Path; }
        
        void LoadSettings();
        void SaveSettings();

    private:
        
        FString StartupProject;
    };
}
