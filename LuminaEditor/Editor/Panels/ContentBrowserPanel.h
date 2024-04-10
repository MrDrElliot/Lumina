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

        std::string CurrentPath = "../";
        std::string SelectedDirectory;
        
    };
}
