﻿#include "SceneEditorTool.h"

#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "EditorToolContext.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Object/ObjectIterator.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/NameComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"
#include "Scene/Entity/Components/LightComponent.h"
#include "Scene/Entity/Components/StaticMeshComponent.h"
#include "Scene/Entity/Components/VelocityComponent.h"
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
                if (EntityListItem->GetEntity().HasComponent<SEditorComponent>())
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

                        float const tableHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
                        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(40, 40, 40, 255));
                        if (ImGui::BeginTable("Options List", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetContentRegionAvail().x, tableHeight)))
                        {
                            ImGui::PushID(Item);
                            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);

                            for (TObjectIterator<CStruct> It; It; ++It)
                            {
                                CStruct* Struct = *It;
                                if (Struct->IsChildOf(SEntityComponent::StaticStruct()))
                                {
                                    Components::ComponentAddFn Fn = Components::GetEntityComponentCreationFn(Struct);
                                    if (Fn)
                                    {
                                        if (ImGui::Selectable(Struct->GetName().c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                                        {
                                            Fn(Scene->GetMutableEntityRegistry(), SelectedEntity.GetHandle());
                                            bComponentAdded = true;
                                        }
                                    }
                                }
                            }
                            
                            ImGui::PopID();
                            ImGui::EndTable();
                        }
                        
                        ImGui::PopStyleColor();

                        if (ImGui::Button("Cancel"))
                        {
                            return true;
                        }
                        
                        if (bComponentAdded)
                        {
                            RebuildPropertyTables();
                        }
                        
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
            for (auto entity : Scene->GetConstEntityRegistry().view<SNameComponent>(entt::exclude<SHiddenComponent>))
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

            RebuildPropertyTables();
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
        if (ImGui::BeginMenu(LE_ICON_CAMERA_CONTROL" Camera Control"))
        {
            float Speed = EditorEntity.GetComponent<SVelocityComponent>().Speed;
            ImGui::SliderFloat("Camera Speed", &Speed, 1.0f, 200.0f);
            EditorEntity.GetComponent<SVelocityComponent>().Speed = Speed;
            ImGui::EndMenu();
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
            ImGui::Text("Mesh Proxies");   ImGui::SameLine(150); ImGui::Text("%llu", SceneRenderer->GetNumMeshProxies());

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
    
        SCameraComponent& CameraComponent = EditorEntity.GetComponent<SCameraComponent>();
        STransformComponent& TransformComponent = SelectedEntity.GetComponent<STransformComponent>();
    
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
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
        if (ImGui::Button("Create New Entity", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
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

        ImGui::BeginChild("EntityEditor", ImGui::GetContentRegionAvail(), true);

        for (FPropertyTable* Table : PropertyTables)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));

            ImVec2 cursor = ImGui::GetCursorScreenPos();
            ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight());
            ImU32 bgColor = IM_COL32(60, 60, 60, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + size.y), bgColor);

            ImGui::PushStyleColor(ImGuiCol_Header,        0);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive,  0);

            if (ImGui::CollapsingHeader(Table->GetType()->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                Table->DrawTree();
                ImGui::Unindent();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);

            ImGui::Spacing();
        }

        ImGui::EndChild();
    }

    void FSceneEditorTool::DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FSceneEditorTool::RebuildPropertyTables()
    {
        for (FPropertyTable* Table : PropertyTables)
        {
            Memory::Delete(Table);
        }

        PropertyTables.clear();

        if (SelectedEntity)
        {
            for (auto&& Curr : Scene->GetMutableEntityRegistry().storage())
            {
                auto& Storage = Curr.second;
                if (Storage.contains(SelectedEntity.GetHandle()))
                {
                    void* ComponentPtr = Storage.value(SelectedEntity.GetHandle());
                    if (ComponentPtr == nullptr)
                    {
                        continue;
                    }
                    
                    SEntityComponent* EntityComponent = (SEntityComponent*)ComponentPtr;
                    CStruct* Type = EntityComponent->GetType();
                    if (Type != nullptr)
                    {
                        PropertyTables.emplace_back(Memory::New<FPropertyTable>(ComponentPtr, Type))->RebuildTree();
                    }
                }
            }
        }
    }

    void FSceneEditorTool::CreateEntity()
    {
        Scene->CreateEntity(FTransform(), FName("New Entity"));
        OutlinerListView.MarkTreeDirty();
    }
}
