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

            virtual ~FEntityListViewItem() = default;

            const char* GetTooltipText() const override { return GetName().c_str(); }
            bool HasContextMenu() override { return true; }

            const FName& GetName() const override
            {
                return Entity.GetConstComponent<FNameComponent>().GetName();
            }
        
            
        private:

            Entity Entity;
        };
        
        
    public:
        
        FSceneEditorTool(const IEditorToolContext* Context, FScene* InScene);

        void OnInitialize(const FUpdateContext& UpdateContext) override;
        void OnDeinitialize(const FUpdateContext& UpdateContext) override { }

        void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const override;
        
    protected:

        void DrawOutliner(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawEntityEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void DrawPropertyEditor(const FUpdateContext& UpdateContext, bool bFocused);

        void CreateEntity();


    private:

        FTreeListView               OutlinerListView;
        FTreeListViewContext        OutlinerContext;
    };
}
