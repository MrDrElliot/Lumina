#pragma once
#include "EditorTool.h"
#include "Scene/Entity/Systems/EntitySystem.h"
#include "Tools/UI/ImGui/Widgets/TreeListView.h"
#include "UI/Properties/PropertyTable.h"

namespace Lumina
{
    /**
     * Base class for display and manipulating scenes.
     */
    class FSceneEditorTool : public FEditorTool
    {

        LUMINA_SINGLETON_EDITOR_TOOL(FSceneEditorTool)


        class FEntityListViewItem : public FTreeListViewItem
        {
        public:

            FEntityListViewItem(FTreeListViewItem* InParent, Entity InEntity)
                : FTreeListViewItem(InParent)
                , Entity(InEntity)
            {}

            virtual ~FEntityListViewItem() override { }

            const char* GetTooltipText() const override { return GetName().c_str(); }
            bool HasContextMenu() override { return true; }

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

            virtual ~FSystemListViewItem() override { }

            const char* GetTooltipText() const override { return GetName().c_str(); }
            bool HasContextMenu() override { return true; }

            FName GetName() const override
            {
                return System->GetName();
            }

            CEntitySystem* GetSystem() const { return System; }
            
        private:

            TObjectHandle<CEntitySystem> System;
        };
        
    public:
        
        FSceneEditorTool(IEditorToolContext* Context, FScene* InScene);

        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override { }

        void Update(const FUpdateContext& UpdateContext) override;

        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

        void DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture, ImVec2 ViewportSize) override;

        void PushAddComponentModal(const Entity& Entity);
        void PushAddSystemModal();
        void PushRenameEntityModal(Entity Ent);
        
    protected:

        void DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused);
        void DrawSystems(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void RebuildPropertyTables();
        void CreateEntity();
        void CreateSystem(CClass* SystemClass);

        void CopyEntity(Entity& To, const Entity& From);

    private:

        ImGuizmo::OPERATION         GuizmoOp;
        
        Entity                      SelectedEntity;
        Entity                      CopiedEntity;
        
        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;

        FTreeListView               SystemsListView;
        FTreeListViewContext        SystemsContext;
        
        TQueue<Entity>              EntityDestroyRequests;
        TVector<FPropertyTable*>    PropertyTables;
    };
}
