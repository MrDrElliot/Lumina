#pragma once
#include "EditorPanel.h"

#include "imgui.h"
#include <Plugins/ImFileBrowser.h>


namespace Lumina
{
    class FContentBrowserPanel : public FEditorPanel
    {
    public:
        FContentBrowserPanel();
        ~FContentBrowserPanel();

        void OnAdded() override;
        void OnRemoved() override;
        void OnImGui() override;
        void OnNewScene()override;
        void OnRender() override;        

    private:

        std::unique_ptr<ImGui::FileBrowser> FileBrowser;
        
    };
}
