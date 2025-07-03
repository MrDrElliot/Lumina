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
            CTexture* Texture = Cast<CTexture>(Asset);
            FRenderManager* RenderManager = Cxt.GetSubsystem<FRenderManager>();
            ImTextureID TextureID = RenderManager->GetImGuiRenderer()->GetOrCreateImTexture(Texture->RHIImage);
            

            ImVec2 WindowSize = ImGui::GetContentRegionAvail();
            ImVec2 WindowPos = ImGui::GetCursorScreenPos();

            if (ImGui::IsWindowHovered())
            {
                float Wheel = ImGui::GetIO().MouseWheel;
                ZoomFactor += Wheel * (ImGui::GetIO().KeyCtrl ? 0.025f : 0.25f);
                ZoomFactor = ImClamp(ZoomFactor, 1.0f, 10.0f);

                if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
                {
                    ImVec2 MouseDelta = ImGui::GetIO().MouseDelta;
                    PanOffset.x += MouseDelta.x;
                    PanOffset.y += MouseDelta.y;
                }
            }

            ImVec2 TextureSize = ImVec2(512.0f, 512.0f);
            ImVec2 ScaledSize = ImVec2(TextureSize.x * ZoomFactor, TextureSize.y * ZoomFactor);
            ImVec2 CenterPos = ImVec2(
                WindowPos.x + (WindowSize.x - ScaledSize.x) * 0.5f + PanOffset.x,
                WindowPos.y + (WindowSize.y - ScaledSize.y) * 0.5f + PanOffset.y
            );

            ImDrawList* DrawList = ImGui::GetWindowDrawList();
            DrawList->AddImage(
                TextureID,
                CenterPos,
                ImVec2(CenterPos.x + ScaledSize.x, CenterPos.y + ScaledSize.y)
            );

            ImGui::SetCursorScreenPos(ImVec2(WindowPos.x, WindowPos.y + WindowSize.y - 20));
            ImGui::Text("Zoom: %.2fx", ZoomFactor);
        });

    
        CreateToolWindow(TexturePropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CTexture* Texture = Cast<CTexture>(Asset);
        
            ImGui::Text("Texture: %s", Texture->GetName().c_str());
            ImGui::Text("Num Pixels: %zu", Texture->Pixels.size());
            
            const FRHIImageDesc& ImageDesc = Texture->ImageDescription;
        
            ImGui::Text("Extent: %dx%d", ImageDesc.Extent.X, ImageDesc.Extent.Y);
            ImGui::Text("Depth: %d", ImageDesc.Depth);
            ImGui::Text("Array Size: %d", ImageDesc.ArraySize);
            ImGui::Text("Num Mips: %d", ImageDesc.NumMips);
            ImGui::Text("Num Samples: %d", ImageDesc.NumSamples);
            ImGui::Text("Dimension: %s", (ImageDesc.Dimension == EImageDimension::Texture2D) ? "2D" :
                                                    (ImageDesc.Dimension == EImageDimension::Texture3D) ? "3D" :
                                                    (ImageDesc.Dimension == EImageDimension::TextureCube) ? "Cube" : "Unknown");
            ImGui::Text("Format: %hhu", static_cast<uint8>(ImageDesc.Format));

            std::string flagsText;
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::ShaderResource)) flagsText += "ShaderResource ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::RenderTarget)) flagsText += "RenderTarget ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::DepthStencil)) flagsText += "DepthStencil ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::Storage)) flagsText += "Storage ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::InputAttachment)) flagsText += "InputAttachment ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::UnorderedAccess)) flagsText += "UnorderedAccess ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::CubeCompatible)) flagsText += "CubeCompatible ";
            if (ImageDesc.Flags.IsFlagSet(EImageCreateFlags::Aliasable)) flagsText += "Aliasable ";
        
            if (!flagsText.empty())
            {
                ImGui::Text("Flags: %s", flagsText.c_str());
            }
            else
            {
                ImGui::Text("Flags: None");
            }
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
