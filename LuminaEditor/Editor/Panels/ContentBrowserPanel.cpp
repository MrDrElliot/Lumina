#include "ContentBrowserPanel.h"

#include "imgui_internal.h"

namespace Lumina
{
    FContentBrowserPanel::FContentBrowserPanel()
    {
        Name = "Content Browser";

        FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_NoTitleBar);

        FileBrowser->SetTitle("Content Browser");
        FileBrowser->Open();
    }

    FContentBrowserPanel::~FContentBrowserPanel()
    {
        
    }

    void FContentBrowserPanel::OnAdded()
    {
        
    }

    void FContentBrowserPanel::OnRemoved()
    {
    }

    void FContentBrowserPanel::OnImGui()
    {
        FileBrowser->Display();
        
        if(FileBrowser->HasSelected())
        {
            FileBrowser->ClearSelected();
        }
    }

    void FContentBrowserPanel::OnNewScene()
    {
    }

    void FContentBrowserPanel::OnRender()
    {
    }
}
