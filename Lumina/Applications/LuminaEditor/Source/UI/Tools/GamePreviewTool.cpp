#include "GamePreviewTool.h"

#include "Core/Engine/Engine.h"
#include "World/WorldManager.h"

namespace Lumina
{
    FGamePreviewTool::FGamePreviewTool(IEditorToolContext* Context, CWorld* InWorld)
        :FEditorTool(Context, "Game Preview", InWorld)
    {
    }

    void FGamePreviewTool::OnInitialize()
    {
    }

    void FGamePreviewTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FGamePreviewTool::Update(const FUpdateContext& UpdateContext)
    {
        
    }

    void FGamePreviewTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        
    }

    void FGamePreviewTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), InDockspaceID);
    }

    void FGamePreviewTool::DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureRef ViewportTexture, ImVec2 ViewportSize)
    {
        
    }
}
