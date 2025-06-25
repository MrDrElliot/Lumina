#include "TextureEditorTool.h"

#include "Core/Object/Class.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Object/Cast.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    const char* TexturePreviewName           = "TexturePreview";
    const char* TexturePropertiesName        = "TextureProperties";

    void FTextureEditorTool::OnInitialize()
    {
        CreateToolWindow(TexturePreviewName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            ImGui::Text("Test");
            CTexture* Texture = Cast<CTexture>(Asset);
            FRenderManager* RenderManager = Cxt.GetSubsystem<FRenderManager>();
            ImTextureID TextureID = RenderManager->GetImGuiRenderer()->GetOrCreateImTexture(Texture->RHIImage);
            ImGui::Image(TextureID, {(float)Texture->ImageDescription.Extent.X, (float)Texture->ImageDescription.Extent.Y});
            ImGui::Text("Test");
        });

        CreateToolWindow(TexturePropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CTexture* Texture = Cast<CTexture>(Asset);

            ImGui::Text("Texture: %s", Texture->GetName().c_str());
            ImGui::Text("Num Pixels: %i", Texture->Pixels.size());
        });
        
    }

    void FTextureEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FTextureEditorTool::OnAssetLoadFinished()
    {
    }

    void FTextureEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        FAssetEditorTool::DrawToolMenu(UpdateContext);
    }

    void FTextureEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(TexturePreviewName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(TexturePropertiesName).c_str(), rightDockID);

    }
}
