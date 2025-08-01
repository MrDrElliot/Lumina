﻿
#include "EditorTool.h"
#include "imgui_internal.h"
#include "ToolFlags.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"
#include "Scene/Entity/Components/VelocityComponent.h"
#include "Scene/Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    FEditorTool::FEditorTool(IEditorToolContext* Context, const FString& DisplayName, FScene* InScene)
        : ToolContext(Context)
        , ToolName(DisplayName)
        , Scene(InScene)
        , EditorEntity()
    {
        ToolFlags |= EEditorToolFlags::Tool_WantsToolbar;
    }

    void FEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGui::DockBuilderRemoveNodeChildNodes(InDockspaceID);
    }

    void FEditorTool::Initialize()
    {
        SetDisplayName(ToolName);
        
        if (HasScene())
        {
            EditorEntity = Scene->CreateEntity(FTransform(), FName("Editor Camera"));
            EditorEntity.AddComponent<SCameraComponent>();
            EditorEntity.AddComponent<SEditorComponent>();
            EditorEntity.AddComponent<SVelocityComponent>().Speed = 50.0f;
            EditorEntity.AddComponent<SHiddenComponent>();
            EditorEntity.GetComponent<STransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, 2.0f));
            
            
            Scene->GetSceneCameraManager()->SetActiveCamera(EditorEntity);
            
            FToolWindow* NewWindow = CreateToolWindow(ViewportWindowName, [] (const FUpdateContext& Contxt, bool bIsFocused)
            {
                //... Intentionally blank.
            });

            NewWindow->bViewport = true;
        }

        OnInitialize();
    }

    void FEditorTool::Deinitialize(const FUpdateContext& UpdateContext)
    {
        OnDeinitialize(UpdateContext);
        
        for (FToolWindow* Window : ToolWindows)
        {
            Memory::Delete(Window);
        }
        
        if (HasScene())
        {
            UpdateContext.GetSubsystem<FSceneManager>()->DestroyScene(Scene);
        }
        
        ToolWindows.clear();
    }

    void FEditorTool::DrawMainToolbar(const FUpdateContext& UpdateContext)
    {
        if (ImGui::MenuItem(LE_ICON_FILE_PLUS_OUTLINE"##New"))
        {
            OnNew();
        }

        if (ImGui::MenuItem(LE_ICON_CONTENT_SAVE"##Save"))
        {
            FTaskSystem::Get()->ScheduleLambda(1, [this](uint32 Start, uint32 End, uint32 Thread)
            {
                OnSave();
            });
        }

        if (ImGui::MenuItem(LE_ICON_UNDO_VARIANT"##Undo"))
        {
            OnUndo();
        }
        ImGuiX::ItemTooltip( "Undo" );

        //-------------------------------------------------------------------------

        if (ImGui::MenuItem(LE_ICON_REDO_VARIANT"##Redo"))
        {
            
        }
        ImGuiX::ItemTooltip( "Redo" );
        

        if (ImGui::BeginMenu(LE_ICON_HELP_CIRCLE_OUTLINE" Help"))
        {
            if (ImGui::BeginTable( "HelpTable", 2 ))
            {
                DrawHelpMenu(UpdateContext);
                ImGui::EndTable();
            }
            ImGui::EndMenu();
        }

        DrawToolMenu(UpdateContext);
    }

    bool FEditorTool::DrawViewport(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture)
    {
        const ImVec2 ViewportSize(eastl::max(ImGui::GetContentRegionAvail().x, 64.0f), eastl::max(ImGui::GetContentRegionAvail().y, 64.0f));
        const ImVec2 WindowPosition = ImGui::GetWindowPos();
        const ImVec2 WindowBottomRight = { WindowPosition.x + ViewportSize.x, WindowPosition.y + ViewportSize.y };
        float AspectRatio = (ViewportSize.x / ViewportSize.y);
        float t = (ViewportSize.x - 500) / (1200 - 500);
        t = glm::clamp(t, 0.0f, 1.0f);
        float NewFOV = glm::mix(120.0f, 50.0f, t);

        SCameraComponent& CameraComponent = EditorEntity.GetComponent<SCameraComponent>();
        CameraComponent.SetAspectRatio(AspectRatio);
        CameraComponent.SetFOV(NewFOV);
        
        /** Mostly for debug, so we can easily see if there's some transparency issue */
        ImGui::GetWindowDrawList()->AddRectFilled(WindowPosition, WindowBottomRight, IM_COL32(255, 0, 0, 255));
        
        
        if (bViewportHovered)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
            {
                ImGui::SetWindowFocus();
                bViewportFocused = true;
            }
        }

        ImVec2 CursorScreenPos = ImGui::GetCursorScreenPos();
        
        ImGui::GetWindowDrawList()->AddImage(
            ViewportTexture,
            CursorScreenPos,
            ImVec2(CursorScreenPos.x + ViewportSize.x, CursorScreenPos.y + ViewportSize.y),
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32_WHITE
        );

        DrawViewportOverlayElements(UpdateContext, ViewportTexture, ViewportSize);
        
        
        if (ImGuiDockNode* pDockNode = ImGui::GetWindowDockNode())
        {
           pDockNode->LocalFlags = 0;
           pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
           pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        }

        return false;
    }

    FEditorTool::FToolWindow* FEditorTool::CreateToolWindow(const FString& InName, const TFunction<void(const FUpdateContext&, bool)>& DrawFunction, const ImVec2& WindowPadding, bool DisableScrolling)
    {
        for (FToolWindow* Window : ToolWindows)
        {
            Assert(Window->Name != InName)
        }

        auto pToolWindow = ToolWindows.emplace_back(Memory::New<FToolWindow>(InName, DrawFunction, WindowPadding, DisableScrolling));
        eastl::sort(ToolWindows.begin(), ToolWindows.end(), [] (const FToolWindow* pLHS, const FToolWindow* pRHS)
        {
            return pLHS->Name < pRHS->Name;
        });
        
        return pToolWindow;
    }

    void FEditorTool::SetEditorCameraEnabled(bool bNewEnable)
    {
        EditorEntity.GetComponent<SEditorComponent>().bEnabled = bNewEnable;
    }

    void FEditorTool::DrawHelpTextRow(const char* pLabel, const char* pText) const
    {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        {
            ImGui::Text(pLabel);
        }

        ImGui::TableNextColumn();
        {
            ImGui::Text(pText);
        }
    }
    
    void FEditorTool::SetDisplayName(FString NewName)
    {
        Assert(!NewName.empty())
        
        ToolName.sprintf("%s %s", GetTitlebarIcon(), NewName.data());
    }
}
