#pragma once
#include <imgui.h>
#include <Plugins/ImFileBrowser.h>

#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class OpenProjectPanel : public EditorImGuiWindow
    {
    public:

        OpenProjectPanel()
        {
            bVisible = false;
            bShowInWindows = false;
        }
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

    private:

        ImGui::FileBrowser FileBrowser;
    
    };
}
