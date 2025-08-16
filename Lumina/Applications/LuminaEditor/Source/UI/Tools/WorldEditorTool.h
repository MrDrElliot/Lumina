#pragma once
#include "EditorTool.h"
#include "Core/Delegates/Delegate.h"
#include "World/Entity/Systems/EntitySystem.h"
#include "Tools/UI/ImGui/Widgets/TreeListView.h"
#include "UI/Properties/PropertyTable.h"

namespace Lumina
{
    DECLARE_MULTICAST_DELEGATE(FOnGamePreview);
    
    /**
     * Base class for display and manipulating scenes.
     */
    class FWorldEditorTool : public FEditorTool
    {
        using Super = FEditorTool;
        LUMINA_SINGLETON_EDITOR_TOOL(FWorldEditorTool)


        class FEntityListViewItem : public FTreeListViewItem
        {
        public:

            FEntityListViewItem(FTreeListViewItem* InParent, Entity InEntity)
                : FTreeListViewItem(InParent)
                , Entity(InEntity)
            {}
            
            ~FEntityListViewItem() override { }

            constexpr static const char* DragDropID = "EntityItem";
            
            const char* GetTooltipText() const override { return GetName().c_str(); }
            bool HasContextMenu() override { return true; }
            uint64 GetHash() const override { return (uint64)Entity.GetHandle(); }
            void SetDragDropPayloadData() const override
            {
                uintptr_t IntPtr = (uintptr_t)this;
                ImGui::SetDragDropPayload(DragDropID, &IntPtr, sizeof(uintptr_t));
            }
            
            FName GetName() const override
            {
                return Entity.GetConstComponent<SNameComponent>().Name;
            }

            Entity GetEntity() const { return Entity; }
            
        private:

            Entity Entity;
        };
        

        class FSystemListViewItem : public FTreeListViewItem
        {
        public:

            FSystemListViewItem(FTreeListViewItem* InParent, CEntitySystem* InSystem)
                : FTreeListViewItem(InParent)
                , System(InSystem)
            {}

            ~FSystemListViewItem() override { }

            const char* GetTooltipText() const override { return GetName().c_str(); }
            bool HasContextMenu() override { return true; }
            uint64 GetHash() const override { return System->GetSystemID(); }
            FName GetName() const override
            {
                return System->GetName();
            }

            CEntitySystem* GetSystem() const { return System; }
            
        private:

            TObjectHandle<CEntitySystem> System;
        };
        
    public:
        
        FWorldEditorTool(IEditorToolContext* Context, CWorld* InWorld);
        ~FWorldEditorTool() noexcept override { }

        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override;

        void OnSave() override;
        void Update(const FUpdateContext& UpdateContext) override;

        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

        void DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture, ImVec2 ViewportSize) override;
        void DrawViewportToolbar(const FUpdateContext& UpdateContext) override;

        void PushAddComponentModal(const Entity& Entity);
        void PushAddSystemModal();
        void PushRenameEntityModal(Entity Ent);

        
        FOnGamePreview& GetOnPreviewStartRequestedDelegate() { return OnGamePreviewStartRequested; }
        FOnGamePreview& GetOnPreviewStopRequestedDelegate() { return OnGamePreviewStopRequested; }

        void NotifyPlayInEditorStart();
        void NotifyPlayInEditorStop();

        void SetWorld(CWorld* InWorld);
        
    protected:

        void RebuildSceneOutliner(FTreeListView* View);
        void HandleEntityEditorDragDrop(FTreeListViewItem* DropItem);

        void DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused);
        void DrawSystems(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void RebuildPropertyTables();
        void CreateEntity();
        void CreateSystem(CClass* SystemClass);

        void CopyEntity(Entity& To, const Entity& From);

        void CycleGuizmoOp();

    private:

        FOnGamePreview              OnGamePreviewStartRequested;
        FOnGamePreview              OnGamePreviewStopRequested;
        
        ImGuizmo::OPERATION         GuizmoOp;
        ImGuizmo::MODE              GuizmoMode;
        
        Entity                      SelectedEntity;
        Entity                      CopiedEntity;
        
        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;

        FTreeListView               SystemsListView;
        FTreeListViewContext        SystemsContext;
        
        TQueue<Entity>              EntityDestroyRequests;
        TVector<FPropertyTable*>    PropertyTables;

        bool                        bGamePreviewRunning = false;
    };
    
}
