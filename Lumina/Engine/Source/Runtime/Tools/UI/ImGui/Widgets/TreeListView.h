#pragma once
#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Memory/Memory.h"

namespace Lumina
{

    class LUMINA_API FTreeListViewItem
    {

        friend class FTreeListView;
    public:
        
        virtual ~FTreeListViewItem() = default;

        FTreeListViewItem(FTreeListViewItem* InParent)
            : Parent(InParent)
            , bExpanded(false)
            , bVisible(false)
            , bSelected(false)
        {}
        
        virtual FName GetName() const = 0;
        
        virtual const char* GetTooltipText() const { return nullptr; }

        virtual bool HasContextMenu() { return false; }

        virtual ImVec4 GetDisplayColor() const;
        
        virtual void OnSelectionStateChanged() { }

        bool HasChildren() const { return !Children.empty(); }

        virtual void SetDragDropPayloadData() const { }

        virtual uint64 GetHash() const = 0;
        
        virtual FInlineString GetDisplayName() const
        {
            return GetName().c_str();
        }

        template<typename T, typename... Args>
        requires (std::is_base_of_v<FTreeListViewItem, T> && std::is_constructible_v<T, Args...>)
        T* AddChild(Args&&... args)
        {
            T* New = Memory::New<T>(std::forward<Args>(args)...);
            Children.push_back(New);

            return New;
        }

    private:

        // Disable copies/moves
        FTreeListViewItem& operator=(FTreeListViewItem const&) = delete;
        FTreeListViewItem& operator=(FTreeListViewItem&&) = delete;


    protected:
        
        FTreeListViewItem*          Parent = nullptr;
        TVector<FTreeListViewItem*> Children;

        uint8                       bExpanded:1;
        uint8                       bVisible:1;
        uint8                       bSelected:1;
        
    };

    struct LUMINA_API FTreeListViewContext
    {
        /** Callback to draw any context menus this item may want */
        TFunction<void(const TVector<FTreeListViewItem*>&)>         DrawItemContextMenuFunction;

        /** Called when a rebuild of the widget tree is requested */
        TFunction<void(FTreeListView*)>                             RebuildTreeFunction;

        /** Called when an item has been selected in the tree */
        TFunction<void(FTreeListViewItem*)>                         ItemSelectedFunction;

        /** Called when we have a drag-drop operation on a target */
        TFunction<void(FTreeListViewItem*)>                         DragDropFunction;
    };
    
    
    class LUMINA_API FTreeListView
    {
    public:

        FTreeListView()
            : bCurrentlyDrawing(false)
            , bDirty(false)
        {}

        ~FTreeListView()
        {
            ClearTree();
        }

        FTreeListView(const FTreeListView&) = delete;
        FTreeListView& operator=(const FTreeListView&) = delete;
        

        void Draw(const FTreeListViewContext& Context);

        void ClearTree();

        void MarkTreeDirty() { bDirty = true; }
        bool IsCurrentlyDrawing() const { return bCurrentlyDrawing; }
        bool IsDirty() const { return bDirty; }
        
        template<typename T, typename... Args>
        requires (std::is_base_of_v<FTreeListViewItem, T> && std::is_constructible_v<T, Args...>)
        T* AddItemToTree(Args&&... args)
        {
            T* New = Memory::New<T>(eastl::forward<Args>(args)...);
            ListItems.push_back(New);

            return New;
        }
        
        void SetSelection(FTreeListViewItem* Item, FTreeListViewContext Context);
        
    private:

        void RebuildTree(FTreeListViewContext Context);
        
        void DrawListItem(FTreeListViewItem* ItemToDraw, FTreeListViewContext Context);

        void ClearSelection();

        void ForEachItem(const TFunction<void(FTreeListViewItem* Item)>& Functor);

    private:

        TVector<FTreeListViewItem*>             Selections;
        TVector<FTreeListViewItem*>             ListItems;

        uint8                                   bDirty:1;
        uint8                                   bCurrentlyDrawing:1;
    };
    
}




#if 0
int nLeafNum = 300000;
if (ImGui::TreeNodeEx("Large Tree"))
{
    //query window and node info
    ImVec2  vLastItem = ImGui::GetItemRectMax();
    ImVec2  vItemSize = ImGui::GetItemRectSize();
    ImVec2  vWindowPos = ImGui::GetWindowPos();
    ImVec2  vWindowSize = ImGui::GetWindowSize();

    //measure the number of node to draw
    int nLeafStart = eastl::max(int((vWindowPos.y - vLastItem.y) / vItemSize.y), 0);
    int nLeafCanDraw = eastl::min(int(vWindowSize.y / vItemSize.y), (int)nLeafNum - nLeafStart);

    //blank rect for those node beyond window
    if (nLeafStart > 0 && nLeafCanDraw > 0)
    {
        ImGui::Dummy(ImVec2(10.0f, float(nLeafStart) * vItemSize.y));
    }

    //all the node we could see
    int nDrawLeaf = nLeafStart;
    while (nDrawLeaf < nLeafCanDraw+ nLeafStart && nDrawLeaf < nLeafNum)
    {
        auto strLeafName = std::to_string(nDrawLeaf);
        bool bIsKey = nDrawLeaf % 10 == 0;
        ImGui::PushID(0); ImGui::PushStyleColor(ImGuiCol_Text, bIsKey ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
        if (ImGui::TreeNodeEx(strLeafName.c_str(), ImGuiTreeNodeFlags_Leaf))
        {
            ImGui::TreePop();
        }
        ImGui::PopStyleColor(1); ImGui::PopID();
        nDrawLeaf++;
    }

    //blank rect for those node exceed window bottom
    if (nDrawLeaf < nLeafNum)
    {
        ImGui::Dummy(ImVec2(10.0f, float(nLeafNum - nDrawLeaf) * vItemSize.y));
    }
    ImGui::TreePop();
}
#endif