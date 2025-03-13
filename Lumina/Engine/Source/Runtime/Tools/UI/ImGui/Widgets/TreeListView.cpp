#include "TreeListView.h"

#include "imgui.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    ImVec4 FTreeListViewItem::GetDisplayColor() const
    {
        return bSelected ? ImVec4(0.60f, 0.60f, 1.0f, 1.0f) : ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    }

    void FTreeListView::Draw(FTreeListViewContext Context)
    {

        if (bDirty)
        {
            RebuildTree(Context);
            return;
        }
        
        bCurrentlyDrawing = true;
        
        ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_ScrollY;
        TableFlags |= ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersV;

        
        ImGui::PushID(this);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
        if (ImGui::BeginTable("TreeViewTable", 1, TableFlags, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x / 2, -1)))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

            ImGuiListClipper Clipper;
            Clipper.Begin((int)ListItems.size());

            while (Clipper.Step())
            {
                for (int i = Clipper.DisplayStart; i < Clipper.DisplayEnd; ++i)
                {
                    float const CursorPosY = ImGui::GetCursorPosY();
                    DrawListItem(ListItems[i], Context);
                }
            }
            
            ImGui::EndTable();
        }
        
        ImGui::PopStyleVar();
        ImGui::PopID();

        bCurrentlyDrawing = false;
    }

    void FTreeListView::ClearTree()
    {
        for (auto* Item : ListItems)
        {
            FMemory::Delete(Item);
        }
        
        ListItems.clear();
    }

    void FTreeListView::RebuildTree(FTreeListViewContext Context)
    {
        Assert(bCurrentlyDrawing == false);
        Assert(bDirty);

        ClearSelection();
        ClearTree();
        
        Context.RebuildTreeFunction(this);

        bDirty = false;
    }

    void FTreeListView::DrawListItem(FTreeListViewItem* ItemToDraw, FTreeListViewContext Context)
    {
        bool bSelectedItem = VectorContains(Selections, ItemToDraw);
        Assert(bSelectedItem == ItemToDraw->bSelected);
        
        ImGui::PushID(ItemToDraw);
        ImGui::TableNextRow();
        
        ImGui::TableSetColumnIndex(0);

        ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf;

        if (bSelectedItem)
        {
            Flags |= ImGuiTreeNodeFlags_Selected;
        }

        const FInlineString DisplayName = ItemToDraw->GetDisplayName();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ItemToDraw->GetDisplayColor()));
        if (ImGui::TreeNodeEx(ItemToDraw, Flags, "%s", DisplayName.c_str()))
        {
            ImGui::TreePop();
        }
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            SetSelection(ItemToDraw, Context);
        }
        
        const char* TooltipText = ItemToDraw->GetTooltipText();
        if (TooltipText != nullptr)
        {
            ImGuiX::ItemTooltip(TooltipText);
        }

        if (ItemToDraw->HasContextMenu())
        {
            if (ImGui::BeginPopupContextItem("ItemContextMenu"))
            {
                TVector<FTreeListViewItem*> SelectionsToDraw;
                for (FTreeListViewItem* Selection : Selections)
                {
                    if (Selection->HasContextMenu())
                    {
                        SelectionsToDraw.push_back(Selection);
                    }
                }
                
                Context.DrawItemContextMenuFunction(SelectionsToDraw);
                
                ImGui::EndPopup();
            }
        }
        
        ImGui::PopID();
    }

    void FTreeListView::SetSelection(FTreeListViewItem* Item, FTreeListViewContext Context)
    {
        bool bWasSelected = Item->bSelected;
        
        ClearSelection();
        
        if (!bWasSelected)
        {
            Selections.push_back(Item);
            Item->bSelected = true;
        }

        Context.ItemSelectedFunction(Item);
        Item->OnSelectionStateChanged();
    }

    void FTreeListView::ClearSelection()
    {
        for (FTreeListViewItem* Item : Selections)
        {
            Assert(Item->bSelected);
            
            Item->bSelected = false;
            Item->OnSelectionStateChanged();    
        }

        Selections.clear();
    }
}
