#pragma once
#include "EditorTool.h"
#include "Tools/UI/ImGui/Widgets/TreeListView.h"

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
                return Entity.GetConstComponent<FNameComponent>().Name;
            }

            Entity GetEntity() const { return Entity; }
            
        private:

            Entity Entity;
        };
        
        
    public:
        
        FSceneEditorTool(IEditorToolContext* Context, FScene* InScene);

        void OnInitialize() override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override { }

        void Update(const FUpdateContext& UpdateContext) override;

        void DrawToolMenu(const FUpdateContext& UpdateContext) override;
        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;

        void DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture, ImVec2 ViewportSize) override;
    protected:

        void DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void CreateEntity();


    private:

        ImGuizmo::OPERATION         GuizmoOp = ImGuizmo::TRANSLATE;
        Entity                      SelectedEntity;
        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;
        TQueue<Entity>              EntityDestroyRequests;
    };
}
