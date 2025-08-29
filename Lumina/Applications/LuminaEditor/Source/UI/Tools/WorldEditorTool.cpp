#include "WorldEditorTool.h"

#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "EditorToolContext.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Object/ObjectIterator.h"
#include "Core/Object/Package/Package.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "World/SceneRenderer.h"
#include "World/Entity/Components/CameraComponent.h"
#include "World/Entity/Components/NameComponent.h"
#include "World/Entity/Components/EditorComponent.h"
#include "World/Entity/Components/LightComponent.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/Entity/Components/VelocityComponent.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "World/WorldManager.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Systems/DebugCameraEntitySystem.h"


namespace Lumina
{
    static constexpr const char* SystemOutlinerName = "Systems";
    
    FWorldEditorTool::FWorldEditorTool(IEditorToolContext* Context, CWorld* InWorld)
        : FEditorTool(Context, InWorld->GetName().ToString(), InWorld)
        , SelectedEntity()
        , CopiedEntity()
        , OutlinerContext()
        , SystemsContext()
    {
        GuizmoOp = ImGuizmo::TRANSLATE;
        GuizmoMode = ImGuizmo::WORLD;
    }

    void FWorldEditorTool::OnInitialize()
    {
        CreateToolWindow("Outliner", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawOutliner(Context, bisFocused);
        });

        CreateToolWindow(SystemOutlinerName, [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawSystems(Context, bisFocused);
        });
        
        CreateToolWindow("Details", [this] (const FUpdateContext& Context, bool bisFocused)
        {
            DrawEntityEditor(Context, bisFocused);
        });


        //------------------------------------------------------------------------------------------------------

        OutlinerContext.DrawItemContextMenuFunction = [this](const TVector<FTreeListViewItem*>& Items)
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
                    PushAddComponentModal(EntityListItem->GetEntity());
                }
                
                if (ImGui::MenuItem("Rename"))
                {
                    PushRenameEntityModal(EntityListItem->GetEntity());
                }

                if (ImGui::MenuItem("Duplicate"))
                {
                    Entity New;
                    CopyEntity(New, SelectedEntity);
                }
                
                if (ImGui::MenuItem("Delete"))
                {
                    EntityDestroyRequests.push(EntityListItem->GetEntity());
                }
            }
        };

        OutlinerContext.RebuildTreeFunction = [this](FTreeListView* Tree)
        {
            RebuildSceneOutliner(Tree);
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

        OutlinerContext.DragDropFunction = [this](FTreeListViewItem* Item)
        {
            HandleEntityEditorDragDrop(Item);  
        };

        OutlinerListView.MarkTreeDirty();

        //------------------------------------------------------------------------------------------------------


        SystemsContext.RebuildTreeFunction = [this](FTreeListView* Tree)
        {
            for (uint8 i = 0; i < (uint8)EUpdateStage::Max; ++i)
            {
                for (CEntitySystem* System : World->GetSystemsForUpdateStage((EUpdateStage)i))
                {
                    SystemsListView.AddItemToTree<FSystemListViewItem>(nullptr, eastl::move(System));
                }
            }
        };

        SystemsListView.MarkTreeDirty();
        
    }

    void FWorldEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
        GEngine->GetEngineSubsystem<FWorldManager>()->RemoveWorld(World);
        World.MarkGarbage();
    }

    void FWorldEditorTool::OnSave()
    {
        FString FullPath = Paths::ResolveVirtualPath(World->GetPathName());
        Paths::AddPackageExtension(FullPath);

        if (CPackage::SavePackage(World->GetPackage(), World, FullPath.c_str()))
        {
            ImGuiX::Notifications::NotifySuccess("Successfully saved package: \"%s\"", World->GetPathName().c_str());
        }
        else
        {
            ImGuiX::Notifications::NotifyError("Failed to save package: \"%s\"", World->GetPathName().c_str());
        }
    }

    void FWorldEditorTool::Update(const FUpdateContext& UpdateContext)
    {
        while (!EntityDestroyRequests.empty())
        {
            Entity Entity = EntityDestroyRequests.back();
            EntityDestroyRequests.pop();

            if (Entity == SelectedEntity)
            {
                if (CopiedEntity == SelectedEntity)
                {
                    CopiedEntity = {};
                }
                
                SelectedEntity = {};
            }
            
            World->DestroyEntity(Entity);
            
            OutlinerListView.MarkTreeDirty();

        }

        if (SelectedEntity.IsValid())
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))
            {
                CopiedEntity = SelectedEntity;
            }

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_D))
            {
                Entity New;
                CopyEntity(New, SelectedEntity);
            }
        }

        if (CopiedEntity)
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
            {
                Entity New;
                CopyEntity(New, CopiedEntity);
            }
        }

        if (SelectedEntity)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                EntityDestroyRequests.push(SelectedEntity);
            }
        }
    }

    void FWorldEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::BeginMenu(LE_ICON_CAMERA_CONTROL" Camera Control"))
        {
            float Speed = EditorEntity.GetComponent<SVelocityComponent>().Speed;
            ImGui::SliderFloat("Camera Speed", &Speed, 1.0f, 200.0f);
            EditorEntity.GetComponent<SVelocityComponent>().Speed = Speed;
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu(LE_ICON_MOVE_RESIZE " Gizmo Control"))
        {
            static int currentOpIndex = 0;

            const char* operations[] = { "Translate", "Rotate", "Scale" };
            constexpr int operationsCount = IM_ARRAYSIZE(operations);

            switch (GuizmoOp)
            {
            case ImGuizmo::TRANSLATE: currentOpIndex = 0; break;
            case ImGuizmo::ROTATE:    currentOpIndex = 1; break;
            case ImGuizmo::SCALE:     currentOpIndex = 2; break;
            default:                  currentOpIndex = 0; break;
            }

            if (ImGui::Combo("Operation", &currentOpIndex, operations, operationsCount))
            {
                switch (currentOpIndex)
                {
                case 0: GuizmoOp = ImGuizmo::TRANSLATE; break;
                case 1: GuizmoOp = ImGuizmo::ROTATE;    break;
                case 2: GuizmoOp = ImGuizmo::SCALE;     break;
                }
            }

            static int currentModeIndex = 0;

            const char* modes[] = { "World", "Local" };
            constexpr int modesCount = IM_ARRAYSIZE(modes);

            switch (GuizmoMode)
            {
            case ImGuizmo::WORLD: currentModeIndex = 0; break;
            case ImGuizmo::LOCAL: currentModeIndex = 1; break;
            default:              currentModeIndex = 0; break;
            }

            if (ImGui::Combo("Mode", &currentModeIndex, modes, modesCount))
            {
                switch (currentModeIndex)
                {
                case 0: GuizmoMode = ImGuizmo::WORLD; break;
                case 1: GuizmoMode = ImGuizmo::LOCAL; break;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(LE_ICON_DEBUG_STEP_INTO " Render Debug"))
        {
            FSceneRenderer* SceneRenderer = World->GetRenderer();
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
                "Material",
                "Depth",
                "SSAO",
            };

            ESceneRenderGBuffer DebugMode = SceneRenderer->GetGBufferDebugMode();
            int DebugModeInt = static_cast<int>(DebugMode);
            ImGui::PushItemWidth(200);
            if (ImGui::Combo("GBuffer Mode", &DebugModeInt, GBufferDebugLabels, IM_ARRAYSIZE(GBufferDebugLabels)))
            {
                SceneRenderer->SetGBufferDebugMode(static_cast<ESceneRenderGBuffer>(DebugModeInt));
            }
            ImGui::PopItemWidth();


            ImGui::Checkbox("Draw AABB", &SceneRenderer->GetSceneRenderSettings().bDrawAABB);

            ImGui::EndMenu();
        }
    }

    void FWorldEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID dockLeft = 0;
        ImGuiID dockRight = 0;

        // 1. Split root dock vertically: left = viewport, right = other panels
        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.25f, &dockRight, &dockLeft);

        ImGuiID dockRightTop = 0;
        ImGuiID dockRightBottom = 0;

        // 2. Split right dock horizontally into Outliner (top 25%) and bottom (Details + SystemOutliner)
        ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.25f, &dockRightTop, &dockRightBottom);

        ImGuiID dockRightBottomLeft = 0;
        ImGuiID dockRightBottomRight = 0;

        // 3. Split bottom right dock horizontally into Details (left) and SystemOutliner (right)
        ImGui::DockBuilderSplitNode(dockRightBottom, ImGuiDir_Right, 0.5f, &dockRightBottomRight, &dockRightBottomLeft);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), dockLeft);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Outliner").c_str(), dockRightTop);
        ImGui::DockBuilderDockWindow(GetToolWindowName("Details").c_str(), dockRightBottomLeft);
        ImGui::DockBuilderDockWindow(GetToolWindowName(SystemOutlinerName).c_str(), dockRightBottomRight);
    }

    void FWorldEditorTool::DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture, ImVec2 ViewportSize)
    {
        if (SelectedEntity.IsValid() == false)
        {
            return;
        }

        if (bViewportHovered)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Space))
            {
                CycleGuizmoOp();
            }
        }
        
        ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
    
        SCameraComponent& CameraComponent = EditorEntity.GetComponent<SCameraComponent>();
    
        glm::mat4 Matrix = SelectedEntity.GetWorldMatrix();
        glm::mat4 ViewMatrix = CameraComponent.GetViewMatrix();
        glm::mat4 ProjectionMatrix = CameraComponent.GetProjectionMatrix();

        if (CameraComponent.GetViewVolume().GetFrustum().IsInside(SelectedEntity.GetWorldTransform().Location))
        {
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ViewportSize.x, ViewportSize.y);
        
            ImGuizmo::Manipulate(glm::value_ptr(ViewMatrix),
                                 glm::value_ptr(ProjectionMatrix),
                                 GuizmoOp,
                                 GuizmoMode,
                                 glm::value_ptr(Matrix));
    
            if (ImGuizmo::IsUsing())
            {
                glm::mat4 worldMatrix = Matrix;
        
                // Check for parent transform
                if (SelectedEntity.IsChild())
                {
                    glm::mat4 parentWorldMatrix = SelectedEntity.GetParent().GetWorldTransform().GetMatrix();
                    glm::mat4 parentWorldInverse = glm::inverse(parentWorldMatrix);
                   
                    // Convert world transform to local transform by applying inverse parent transform
                    glm::mat4 localMatrix = parentWorldInverse * worldMatrix;
        
                    // Decompose local matrix instead of world
                    glm::vec3 translation, scale, skew;
                    glm::quat rotation;
                    glm::vec4 perspective;
        
                    glm::decompose(localMatrix, scale, rotation, translation, skew, perspective);
        
                    STransformComponent& TransformComponent = SelectedEntity.GetComponent<STransformComponent>();
        
                    if (translation != TransformComponent.GetLocation() ||
                        rotation != TransformComponent.GetRotation() ||
                        scale != TransformComponent.GetScale())
                    {
                        SelectedEntity.Patch<STransformComponent>([&](auto& Transform)
                        {
                            Transform.SetLocation(translation);
                            Transform.SetRotation(rotation);
                            Transform.SetScale(scale);
                        });
                    }
                }
                else
                {
                    // No parent, set world transform as local directly
                    glm::vec3 translation, scale, skew;
                    glm::quat rotation;
                    glm::vec4 perspective;
        
                    glm::decompose(worldMatrix, scale, rotation, translation, skew, perspective);
        
                    STransformComponent& TransformComponent = SelectedEntity.GetComponent<STransformComponent>();
        
                    if (translation != TransformComponent.GetLocation() ||
                        rotation != TransformComponent.GetRotation() ||
                        scale != TransformComponent.GetScale())
                    {
                        /** In the editor, we always mark the currently selected entity as needing transform updates */
                        SelectedEntity.Emplace<FDirtyTransform>();
                        
                        SelectedEntity.Patch<STransformComponent>([&](auto& Transform)
                        {
                            Transform.SetLocation(translation);
                            Transform.SetRotation(rotation);
                            Transform.SetScale(scale);
                        });
                    }
                }
            }
        }
    }

    void FWorldEditorTool::DrawViewportToolbar(const FUpdateContext& UpdateContext)
    {
        Super::DrawViewportToolbar(UpdateContext);

        ImGui::SameLine();
        
        constexpr float ButtonWidth = 120;

        if (!bGamePreviewRunning)
        {
            if (ImGuiX::IconButton(LE_ICON_PLAY, "Play Map", 4278255360, ImVec2(ButtonWidth, 0)))
            {
                OnGamePreviewStartRequested.Broadcast();
            }
        }
        else
        {
            if (ImGuiX::IconButton(LE_ICON_STOP, "Stop Playing", 4278190335, ImVec2(ButtonWidth, 0)))
            {
                OnGamePreviewStopRequested.Broadcast();
            }
        }
    }

    void FWorldEditorTool::PushAddComponentModal(const Entity& Entity)
    {
        ToolContext->PushModal("Add Component", ImVec2(600.0f, 350.0f), [this, Entity](const FUpdateContext& Context) -> bool
        {
            bool bComponentAdded = false;

            float const tableHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
            ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(40, 40, 40, 255));
            if (ImGui::BeginTable("Options List", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetContentRegionAvail().x, tableHeight)))
            {
                ImGui::PushID((int)Entity.GetHandle());
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                for(auto &&[id, type]: entt::resolve())
                {
                    using namespace entt::literals;
                    std::string StringName(type.info().name());
                    if (ImGui::Selectable(StringName.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        void* RegistryPtr = &World->GetMutableEntityRegistry(); // EnTT will try to make a copy if not passed by *.
                        (void)type.invoke("addcomponent"_hs, {}, SelectedEntity.GetHandle(), RegistryPtr);
                        bComponentAdded = true;
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

    void FWorldEditorTool::PushAddSystemModal()
    {
        ToolContext->PushModal("Add System", ImVec2(600.0f, 350.0f), [this](const FUpdateContext& Context) -> bool
        {
            bool bComponentAdded = false;

            float const tableHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
            ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(40, 40, 40, 255));
            if (ImGui::BeginTable("Options List", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetContentRegionAvail().x, tableHeight)))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                TVector<TObjectHandle<CEntitySystem>> Systems;
                CEntitySystemRegistry::Get().GetRegisteredSystems(Systems);

                for (CEntitySystem* EntitySystem : Systems)
                {
                    if (ImGui::Selectable(EntitySystem->GetClass()->GetName().c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        CreateSystem(EntitySystem->GetClass());
                    }
                }
                
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

    void FWorldEditorTool::PushRenameEntityModal(Entity Ent)
    {
        ToolContext->PushModal("Rename Entity", ImVec2(600.0f, 350.0f), [this, Ent](const FUpdateContext& Context) -> bool
        {
            Entity CopyEntity = Ent;
            FName& Name = CopyEntity.EmplaceOrReplace<SNameComponent>().Name;
            FString CopyName = Name.ToString();
            
            if (ImGui::InputText("##Name", const_cast<char*>(CopyName.c_str()), 256, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                Name = CopyName.c_str();
                return true;
            }
            
            if (ImGui::Button("Cancel"))
            {
                return true;
            }

            return false;
        });
    }

    void FWorldEditorTool::NotifyPlayInEditorStart()
    {
        bGamePreviewRunning = true;
    }

    void FWorldEditorTool::NotifyPlayInEditorStop()
    {
         bGamePreviewRunning = false;
    }

    void FWorldEditorTool::SetWorld(CWorld* InWorld)
    {
        if (World == InWorld)
        {
            return;
        }
        
        if (World.IsValid())
        {
            GEngine->GetEngineSubsystem<FWorldManager>()->RemoveWorld(World);
            World.MarkGarbage();
        }
        
        World = InWorld;
        GEngine->GetEngineSubsystem<FWorldManager>()->AddWorld(World);

        World->InitializeWorld();
        EditorEntity = World->SetupEditorWorld();
        
        SelectedEntity = {};
        OutlinerListView.MarkTreeDirty();
    }

    void FWorldEditorTool::RebuildSceneOutliner(FTreeListView* View)
    {
        TFunction<void(Entity, FEntityListViewItem*)> AddEntityRecursive;
        
        AddEntityRecursive = [&](Entity entity, FEntityListViewItem* ParentItem)
        {
            FEntityListViewItem* Item = nullptr;
            if (ParentItem)
            {
                Item = ParentItem->AddChild<FEntityListViewItem>(ParentItem, eastl::move(entity));
            }
            else
            {
                Item = OutlinerListView.AddItemToTree<FEntityListViewItem>(ParentItem, eastl::move(entity));
            }

            if (SRelationshipComponent* rel = Item->GetEntity().TryGetComponent<SRelationshipComponent>())
            {
                for (SIZE_T i = 0; i < rel->Size; ++i)
                {
                    AddEntityRecursive(rel->Children[i], Item);
                }
            }
        };
        

        for (auto EntityHandle : World->GetConstEntityRegistry().view<SNameComponent>(entt::exclude<SHiddenComponent>))
        {
            Entity entity(EntityHandle, World);

            if (auto* Rel = entity.TryGetComponent<SRelationshipComponent>())
            {
                if (Rel->Parent.IsValid())
                {
                    continue;
                }
            }

            AddEntityRecursive(eastl::move(entity), nullptr);
        }
    }


    void FWorldEditorTool::HandleEntityEditorDragDrop(FTreeListViewItem* DropItem)
    {
        const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(FEntityListViewItem::DragDropID, ImGuiDragDropFlags_AcceptBeforeDelivery);
        if (Payload && Payload->IsDelivery())
        {
            uintptr_t* RawPtr = (uintptr_t*)Payload->Data;
            auto* SourceItem = (FEntityListViewItem*)*RawPtr;
            auto* DestinationItem = (FEntityListViewItem*)DropItem;

            if (SourceItem == DestinationItem)
            {
                return;
            }

            World->ReparentEntity(SourceItem->GetEntity(), DestinationItem->GetEntity());
            
            OutlinerListView.MarkTreeDirty();
        }
    }

    void FWorldEditorTool::DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
        if (ImGui::Button("New Entity", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            CreateEntity();
        }
        ImGui::PopStyleColor();

        ImGui::SeparatorText("Entities");
        ImGui::Text("Num: %i", World->GetMutableEntityRegistry().view<SNameComponent>().size());

        OutlinerListView.Draw(OutlinerContext);
    }

    void FWorldEditorTool::DrawSystems(const FUpdateContext& UpdateContext, bool bFocused)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
        if (ImGui::Button("New System", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            PushAddSystemModal();
        }
        ImGui::PopStyleColor();
        
        SystemsListView.Draw(SystemsContext);
    }

    void FWorldEditorTool::DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        if (SelectedEntity.IsValid() == false)
        {
            ImGui::Text("No entity selected.");
            return;
        }

        ImGui::BeginChild("EntityEditor", ImGui::GetContentRegionAvail(), true);

        if (ImGui::CollapsingHeader(SelectedEntity.GetComponent<SNameComponent>().Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
        {
            ImGui::BeginDisabled();
            int ID = (int)SelectedEntity.GetHandle();
            ImGui::DragInt("ID", &ID);
            ImGui::InputText("Name", (char*)SelectedEntity.GetComponent<SNameComponent>().Name.c_str(), 256);
            ImGui::EndDisabled();

            ImGui::Separator();
        
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
            {
                PushAddComponentModal(SelectedEntity);
            }
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("Destroy", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
            {
                EntityDestroyRequests.push(SelectedEntity);
            }
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::SeparatorText("Components");
            ImGui::Spacing();
        
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
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.15f, 0.15f, 1.0f));
                    ImGui::PushID(Table);
                
                    bool bWasRemoved = false;
                    if (Table->GetType() != STransformComponent::StaticStruct() && Table->GetType() != SNameComponent::StaticStruct())
                    {
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                        {
                            for (const auto& [ID, Set] : World->GetMutableEntityRegistry().storage())
                            {
                                if (Set.contains(SelectedEntity.GetHandle()))
                                {
                                    using namespace entt::literals;

                                    auto ReturnValue = entt::resolve(Set.type()).invoke("staticstruct"_hs, {});
                                    void** Type = ReturnValue.try_cast<void*>();

                                    if (Table->GetType() == *(CStruct**)Type)
                                    {
                                        Set.remove(SelectedEntity.GetHandle());
                                        bWasRemoved = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                
                    ImGui::PopID();
                    ImGui::PopStyleColor();

                    if (bWasRemoved)
                    {
                        ImGui::PopStyleColor(3);
                        ImGui::PopStyleVar(2);
                        RebuildPropertyTables();
                        break;
                    }
                
                
                    ImGui::Indent();
                    Table->DrawTree();
                    ImGui::Unindent();
                }

                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(2);

                ImGui::Spacing();
            }

        }
        
        ImGui::EndChild();
    }

    void FWorldEditorTool::DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused)
    {
        
    }

    void FWorldEditorTool::RebuildPropertyTables()
    {
        using namespace entt::literals;
        for (FPropertyTable* Table : PropertyTables)
        {
            Memory::Delete(Table);
        }

        PropertyTables.clear();

        if (SelectedEntity.IsValid())
        {
            for (const auto& [ID, Set] : World->GetMutableEntityRegistry().storage())
            {
                if (Set.contains(SelectedEntity.GetHandle()))
                {
                    if (auto func = entt::resolve(Set.type()).func("staticstruct"_hs); func)
                    {
                        auto ReturnValue = func.invoke(Set.type());
                        void** Type = ReturnValue.try_cast<void*>();
                        
                        if (Type != nullptr)
                        {
                            void* ComponentPtr = Set.value(SelectedEntity.GetHandle());
                            FPropertyTable* NewTable = Memory::New<FPropertyTable>(ComponentPtr, *(CStruct**)Type);
                            PropertyTables.emplace_back(NewTable)->RebuildTree();
                        }
                    }
                }
            }
        }
    }

    void FWorldEditorTool::CreateEntity()
    {
        World->ConstructEntity("Entity");
        OutlinerListView.MarkTreeDirty();
    }

    void FWorldEditorTool::CreateSystem(CClass* SystemClass)
    {
        CEntitySystem* NewSystem = NewObject<CEntitySystem>(SystemClass);
        World->RegisterSystem(NewSystem);
        
        SystemsListView.MarkTreeDirty();
    }

    void FWorldEditorTool::CopyEntity(Entity& To, const Entity& From)
    {
        World->CopyEntity(To, From);
        OutlinerListView.MarkTreeDirty();
    }

    void FWorldEditorTool::CycleGuizmoOp()
    {
        switch (GuizmoOp)
        {
        case ImGuizmo::TRANSLATE:
            {
                GuizmoOp = ImGuizmo::ROTATE;
            }
            break;
        case ImGuizmo::ROTATE:
            {
                GuizmoOp = ImGuizmo::SCALE;
            }
            break;
        case ImGuizmo::SCALE:
            {
                GuizmoOp = ImGuizmo::TRANSLATE;
            }
            break;
        }
    }
}
