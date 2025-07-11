#include "SceneEditorTool.h"

#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "EditorToolContext.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/NameComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"
#include "Scene/Entity/Components/LightComponent.h"


namespace Lumina
{
    class FSceneRenderer;
    class FSceneManager;

    FSceneEditorTool::FSceneEditorTool(IEditorToolContext* Context, FScene* InScene)
        : FEditorTool(Context, "Scene Editor", InScene)
        , SelectedEntity()
        , OutlinerContext()
    {
        Assert(Scene != nullptr)
    }

    void FSceneEditorTool::OnInitialize()
    {
        CreateToolWindow("Outliner", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawOutliner( Context, bisFocused );
        });
        
        CreateToolWindow("Entity Details", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawEntityEditor( Context, bisFocused );
        });

        OutlinerContext.DrawItemContextMenuFunction = [this](const TVector<FTreeListViewItem*> Items)
        {
            for (FTreeListViewItem* Item : Items)
            {
                FEntityListViewItem* EntityListItem = static_cast<FEntityListViewItem*>(Item);
                if (EntityListItem->GetEntity().HasComponent<FEditorComponent>())
                {
                    continue;
                }

                
                if (ImGui::MenuItem("Add Component"))
                {
                    ToolContext->PushModal("Add Component", ImVec2(600.0f, 350.0f), [this, Item](const FUpdateContext& Context) -> bool
                    {
                        FEntityListViewItem* EntityListItem = static_cast<FEntityListViewItem*>(Item);
                        Entity Ent = EntityListItem->GetEntity();

                        bool bComponentAdded = false;

                        ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_ScrollY;
                        TableFlags |= ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersV;

                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
                        if (ImGui::BeginTable("AddComponentTable", 1, TableFlags, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, -1)))
                        {
                            ImGui::PushID(Item);
                            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);

                            if (ImGui::Selectable("Point Light"))
                            {
                                Ent.AddComponent<FPointLightComponent>();
                                bComponentAdded = true;
                            }

                            ImGui::PopID();
                            ImGui::EndTable();
                        }
                        ImGui::PopStyleVar();

                        return bComponentAdded;
                    });
                }
                
                if (ImGui::MenuItem("Rename"))
                {
                    
                }

                if (ImGui::MenuItem("Delete"))
                {
                    EntityDestroyRequests.push(EntityListItem->GetEntity());
                }
            }
        };

        OutlinerContext.RebuildTreeFunction = [this](FTreeListView* Tree)
        {
            for (auto entity : Scene->GetConstEntityRegistry().view<FNameComponent>(entt::exclude<FHiddenComponent>))
            {
                Entity NewEntity(entity, Scene);
                OutlinerListView.AddItemToTree<FEntityListViewItem>(nullptr, eastl::move(NewEntity));
            }
        };
        
        OutlinerContext.ItemSelectedFunction = [this](FTreeListViewItem* Item)
        {
            if (Item == nullptr)
            {
                SelectedEntity = Entity();
                return;
            }
            
            FEntityListViewItem* EntityListItem = static_cast<FEntityListViewItem*>(Item);
            
            SelectedEntity = EntityListItem->GetEntity();
        };

        OutlinerListView.MarkTreeDirty();
    }

    void FSceneEditorTool::Update(const FUpdateContext& UpdateContext)
    {
        while (!EntityDestroyRequests.empty())
        {
            Scene->DestroyEntity(EntityDestroyRequests.back());
            OutlinerListView.MarkTreeDirty();

            EntityDestroyRequests.pop();
        }
    }

    void FSceneEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::MenuItem(LE_ICON_CAMERA_CONTROL" Camera Control"))
        {
            
        }
        
        // Gizmo Control Dropdown
        if (ImGui::BeginMenu(LE_ICON_MOVE_RESIZE " Gizmo Control"))
        {
            const char* operations[] = { "Translate", "Rotate", "Scale" };
            static int currentOp = 0;

            if (ImGui::Combo("##", &currentOp, operations, IM_ARRAYSIZE(operations)))
            {
                switch (currentOp)
                {
                case 0: GuizmoOp = ImGuizmo::TRANSLATE; break;
                case 1: GuizmoOp = ImGuizmo::ROTATE;    break;
                case 2: GuizmoOp = ImGuizmo::SCALE;     break;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem(LE_ICON_DEBUG_STEP_INTO" Render Graph"))
        {
            FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneManager>()->GetSceneRendererForScene(Scene);
        }
    }

    void FSceneEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID topDockID = 0, bottomLeftDockID = 0, bottomCenterDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.5f, &bottomCenterDockID, &topDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.66f, &bottomCenterDockID, &bottomLeftDockID);
        ImGui::DockBuilderSplitNode(bottomCenterDockID, ImGuiDir_Right, 0.5f, &bottomRightDockID, &bottomCenterDockID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), topDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Outliner").c_str(), bottomLeftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Entity Details").c_str(), bottomCenterDockID);

    }

    void FSceneEditorTool::DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture, ImVec2 ViewportSize)
    {
        if (SelectedEntity.IsValid() == false)
        {
            return;
        }
        
        ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
    
        FCameraComponent& CameraComponent = EditorEntity.GetComponent<FCameraComponent>();
        FTransformComponent& TransformComponent = SelectedEntity.GetComponent<FTransformComponent>();
    
        glm::mat4 Matrix = TransformComponent.Transform.GetMatrix();
    
        glm::mat4 ViewMatrix = CameraComponent.GetViewMatrix();
        glm::mat4 ProjectionMatrix = CameraComponent.GetProjectionMatrix();
        
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ViewportSize.x, ViewportSize.y);
        ImGuizmo::Manipulate(glm::value_ptr(ViewMatrix),
                             glm::value_ptr(ProjectionMatrix),
                             GuizmoOp,
                             ImGuizmo::WORLD,
                             glm::value_ptr(Matrix));
    
        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, scale, Skew;
            glm::quat rotation;
            glm::vec4 Other;
    
            glm::decompose(Matrix, scale, rotation, translation, Skew, Other);
    
            TransformComponent.SetLocation(translation);
            TransformComponent.SetRotation(rotation);
            TransformComponent.SetScale(scale);
        }
    }


    void FSceneEditorTool::DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.65f, 0.15f, 1.0f));
        if (ImGui::Button("Create New Entity", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            CreateEntity();
        }
        ImGui::PopStyleColor();
        
        OutlinerListView.Draw(OutlinerContext);
        
    }

    void FSceneEditorTool::DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        if (SelectedEntity.IsValid() == false)
        {
            ImGui::Text("No entity selected.");
            return;
        }

        ImGui::BeginChild("EntityEditor", ImVec2(0, 200), true);

        ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.0f), "%s", "TODO: Automatic component discovery and property drawing.");
        
        ImGui::EndChild();
    }

    void FSceneEditorTool::DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::CreateEntity()
    {
        Scene->CreateEntity(FTransform(), FName("New Entity"));
        OutlinerListView.MarkTreeDirty();
    }
}
