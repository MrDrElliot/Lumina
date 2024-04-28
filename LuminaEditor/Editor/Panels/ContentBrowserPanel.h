#pragma once
#include "EditorPanel.h"

#include "imgui.h"
#include <Plugins/ImFileBrowser.h>

#include "imgui_internal.h"


namespace Lumina
{
    class FImage;
}

namespace Lumina
{
    class WindowDropEvent;
}

namespace Lumina
{
    class FContentBrowserPanel : public FEditorPanel
    {
    public:
        FContentBrowserPanel();
        ~FContentBrowserPanel();

        void OnAdded() override;
        void OnRemoved() override;
        void OnEvent(FEvent& Event) override;
        void OnNewScene()override;
        void OnRender(double DeltaTime) override;

        bool OnWindowDrop(WindowDropEvent& Event);

    private:

        std::shared_ptr<FImage> FolderImage;
        ImTextureID FolderTexture;
        
        bool bHasBeenDropped = false;
        ImRect WindowSize;
        bool bIsHovered = false;
        std::string CurrentPath = "../Sandbox/";
        std::string SelectedDirectory;
        
    };
}
