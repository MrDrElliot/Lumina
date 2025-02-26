
#include "EditorTool.h"
#include "imgui_internal.h"
#include "ToolFlags.h"
#include "Core/Math/Hash/Hash.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"
#include "Scene/Subsystems/FCameraManager.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    FEditorTool::FEditorTool(const IEditorToolContext* Context, const FString& DisplayName, FScene* InScene)
        : ToolContext(Context)
        , ToolName(DisplayName)
        , ID(Hash::GetHash32(DisplayName))
        , Scene(InScene)
        , EditorEntity()
    {
        ToolFlags |= EEditorToolFlags::Tool_WantsToolbar;
    }

    void FEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGui::DockBuilderRemoveNodeChildNodes(InDockspaceID);
    }

    void FEditorTool::Initialize(const FUpdateContext& UpdateContext)
    {
        SetDisplayName(ToolName);
        
        if (HasScene())
        {
            FTransform EditorEntityTransform = FTransform();
            EditorEntityTransform.Location = {0.0f, 10.0f, 0.0f};
            EditorEntity = Scene->CreateEntity(EditorEntityTransform, FName("Tool_Entity_" + ToolName));
            EditorEntity.AddComponent<FCameraComponent>();
            EditorEntity.AddComponent<FEditorComponent>();
            Scene->GetSceneCameraManager()->SetActiveCamera(EditorEntity);
            
            FToolWindow* NewWindow = CreateToolWindow(ViewportWindowName, [] (const FUpdateContext& Contxt, bool bIsFocused)
            {
                //.. Intentially blank.
            });

            NewWindow->bViewport = true;
        }

        OnInitialize(UpdateContext);
    }

    void FEditorTool::Deinitialize(const FUpdateContext& UpdateContext)
    {
        OnDeinitialize(UpdateContext);
        
        for (FToolWindow* Window : ToolWindows)
        {
            FMemory::Delete(Window);
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
            
        }

        if (ImGui::MenuItem(LE_ICON_CONTENT_SAVE"##Save"))
        {
            
        }

        if (ImGui::MenuItem(LE_ICON_UNDO_VARIANT"##Undo"))
        {
            
        }
        ImGuiX::ItemTooltip( "Undo" );

        //-------------------------------------------------------------------------

        if (ImGui::MenuItem(LE_ICON_REDO_VARIANT"##Redo"))
        {
            
        }
        ImGuiX::ItemTooltip( "Redo" );
        

        if (ImGui::BeginMenu(LE_ICON_HELP_CIRCLE_OUTLINE" Help"))
        {
            if ( ImGui::BeginTable( "HelpTable", 2 ) )
            {
                DrawHelpMenu(UpdateContext);
                ImGui::EndTable();
            }
            ImGui::EndMenu();
        }
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

        EditorEntity.GetComponent<FCameraComponent>().SetAspectRatio(AspectRatio);
        EditorEntity.GetComponent<FCameraComponent>().SetFOV(NewFOV);
        
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
        
        //ImGui::Image(ViewportTexture, ViewportSize);
        
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
        for (FToolWindow* Window : ToolWindows )
        {
            Assert(Window->Name != InName);
        }

        auto pToolWindow = ToolWindows.emplace_back(FMemory::New<FToolWindow>(InName, DrawFunction, WindowPadding, DisableScrolling));
        eastl::sort(ToolWindows.begin(), ToolWindows.end(), [] (const FToolWindow* pLHS, const FToolWindow* pRHS)
        {
            return pLHS->Name < pRHS->Name;
        });
        
        return pToolWindow;
    }

    void FEditorTool::SetEditorCameraEnabled(bool bNewEnable)
    {
        EditorEntity.GetComponent<FEditorComponent>().SetEnabled(bNewEnable);
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
        Assert(!NewName.empty());
        
        ToolName.sprintf("%s %s", GetTitlebarIcon(), NewName.data());
    }
}
