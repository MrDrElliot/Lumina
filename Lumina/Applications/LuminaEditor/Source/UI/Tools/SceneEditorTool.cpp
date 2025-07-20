#include "SceneEditorTool.h"

#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "EditorToolContext.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/NameComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"
#include "Scene/Entity/Components/LightComponent.h"
#include "Scene/Entity/Components/StaicMeshComponent.h"
#include "Tools/UI/ImGui/ImGuiX.h"


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

                            if (ImGui::Selectable("Directional Light"))
                            {
                                Ent.AddComponent<FDirectionalLightComponent>();
                                Ent.AddComponent<FNeedsRenderProxyUpdate>();
                                bComponentAdded = true;
                            }
                            
                            if (ImGui::Selectable("Point Light"))
                            {
                                Ent.AddComponent<FPointLightComponent>();
                                Ent.AddComponent<FNeedsRenderProxyUpdate>();
                                bComponentAdded = true;
                            }

                            if (ImGui::Selectable("Static Mesh"))
                            {
                                Ent.AddComponent<FStaticMeshComponent>();
                                Ent.AddComponent<FNeedsRenderProxyUpdate>();
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

                if (ImGui::MenuItem("Duplicate"))
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

        if (SelectedEntity.IsValid())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
            {
                LOG_INFO("Copied!");
            }
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

        if (ImGui::BeginMenu(LE_ICON_DEBUG_STEP_INTO " Render Debug"))
        {
            FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneManager>()->GetSceneRendererForScene(Scene);
            const FSceneRenderStats& Stats = SceneRenderer->GetSceneRenderStats();

            ImGui::TextColored(ImVec4(1.0f, 0.78f, 0.16f, 1.0f), "Scene Statistics");
            ImGui::Separator();
            ImGui::Text("Draw Calls");    ImGui::SameLine(150); ImGui::Text("%u", Stats.NumDrawCalls);
            ImGui::Text("Vertices");      ImGui::SameLine(150); ImGui::Text("%llu", Stats.NumVertices);
            ImGui::Text("Indices");       ImGui::SameLine(150); ImGui::Text("%llu", Stats.NumIndices);

            ImGui::Spacing();
    
            ImGui::TextColored(ImVec4(0.58f, 0.86f, 1.0f, 1.0f), "Debug Visualization");
            ImGui::Separator();

            static const char* GBufferDebugLabels[] =
            {
                "RenderTarget",
                "Albedo",
                "Position",
                "Normals",
                "Material"
            };

            ESceneRenderGBuffer DebugMode = SceneRenderer->GetGBufferDebugMode();
            int DebugModeInt = static_cast<int>(DebugMode);
            ImGui::PushItemWidth(200);
            if (ImGui::Combo("GBuffer Mode", &DebugModeInt, GBufferDebugLabels, IM_ARRAYSIZE(GBufferDebugLabels)))
            {
                SceneRenderer->SetGBufferDebugMode(static_cast<ESceneRenderGBuffer>(DebugModeInt));
            }
            ImGui::PopItemWidth();

            ImGui::EndMenu();
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

            SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
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

        if (SelectedEntity.HasComponent<FPointLightComponent>())
        {
            auto& Light = SelectedEntity.GetComponent<FPointLightComponent>();

            ImGui::Text("Point Light Component");

            glm::vec3 color = glm::vec3(Light.LightColor);
            float intensity = Light.LightColor.a;

            if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
            {
                Light.LightColor.r = color.r;
                Light.LightColor.g = color.g;
                Light.LightColor.b = color.b;
                SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
            }

            if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 100.0f))
            {
                Light.LightColor.a = intensity;
                SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
            }

            ImGui::Separator();
        }

        if (SelectedEntity.HasComponent<FDirectionalLightComponent>())
        {
            auto& Light = SelectedEntity.GetComponent<FDirectionalLightComponent>();

            ImGui::Text("Directional Light Component");
            float Intensity = Light.Color.a;

            glm::vec4 dir = Light.Direction;
            if (ImGui::SliderFloat3("Direction", &dir.x, -1.0f, 1.0f))
            {
                Light.Direction = dir;
                SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
            }

            glm::vec3 Color = glm::vec3(Light.Color);
            if (ImGui::ColorEdit3("Color", glm::value_ptr(Light.Color)))
            {
                Light.Color.r = Color.r;
                Light.Color.g = Color.g;
                Light.Color.b = Color.b;
                SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
            }
            
            if (ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f))
            {
                Light.Color.a = Intensity;
                SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
            }
        }

        // Static Mesh Component UI
        if (SelectedEntity.HasComponent<FStaticMeshComponent>())
        {
            auto& MeshComp = SelectedEntity.GetComponent<FStaticMeshComponent>();

            float ButtonWidth = ImGui::GetContentRegionAvail().x;

            const char* Label = MeshComp.StaticMesh ? MeshComp.StaticMesh->GetName().c_str() : "nullptr";

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
            ImVec4 BgColor = MeshComp.StaticMesh ? ImVec4(0.2f, 0.25f, 0.3f, 1.0f) : ImVec4(0.3f, 0.1f, 0.1f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, BgColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(BgColor.x + 0.1f, BgColor.y + 0.1f, BgColor.z + 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, BgColor);

            if (ImGui::Button(Label, ImVec2(ButtonWidth, 0)))
            {
                ImGui::OpenPopup("ObjectSelectorPopup");
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);

            if (ImGui::BeginPopup("ObjectSelectorPopup"))
            {
                FARFilter Filter;
                if (ImGuiX::ObjectSelector(Filter, MeshComp.StaticMesh))
                {
                    SelectedEntity.AddComponent<FNeedsRenderProxyUpdate>();
                }
            
                ImGui::EndPopup();
            }
            
            ImGui::Separator();
        }
        
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
