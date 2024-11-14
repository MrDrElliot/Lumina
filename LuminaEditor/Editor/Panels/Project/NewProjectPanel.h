#pragma once
#include <ThirdParty/imgui/imgui.h>
#include <Plugins/ImFileBrowser.h>

#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class NewProjectPanel : public EditorImGuiWindow
    {
    public:

        NewProjectPanel()
        {
            Name = "New Project";
            bVisible = false;
            bShowInWindows = false;
            FileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

    private:

        ImGui::FileBrowser FileDialog;
    };
}
