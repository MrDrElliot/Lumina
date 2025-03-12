#include "TileViewWidget.h"

namespace Lumina
{
    const FName& FTileViewItem::GetName() const
    {
        return {};
    }

    ImVec4 FTileViewItem::GetDisplayColor() const
    {
        return {};
    }

    void FTileViewWidget::Draw(FTileViewContext Context)
    {
        if (bDirty)
        {
            RebuildTree(Context);
            return;
        }
        
        const int columns_per_row = 4;
        const float tile_size = 200.0f;
        const float padding = 10.0f;

        int column_index = 0;
    

        for (FTileViewItem* Item : ListItems)
        {
            ImGui::PushID(Item); // Unique ID for each tile

            // Draw the selectable tile button
            bool bSelected = false;
            if (ImGui::Selectable(Item->GetDisplayName().c_str(), &bSelected, 0, ImVec2(tile_size, tile_size)))
            {
                SetSelection(Item);
            }

            // Draw additional item details (if needed)
            if (ImGui::IsItemHovered() && Item->GetTooltipText())
            {
                ImGui::SetTooltip("%s", Item->GetTooltipText());
            }

            ImGui::PopID();

            // Manage grid layout
            column_index++;
            if (column_index % columns_per_row != 0)
            {
                ImGui::SameLine();
            }
        }

    }

    void FTileViewWidget::ClearTree()
    {
        for (FTileViewItem* Item : ListItems)
        {
            FMemory::Delete(Item);
        }
        
        ListItems.clear();
    }

    void FTileViewWidget::RebuildTree(FTileViewContext Context)
    {
        Assert(bCurrentlyDrawing == false);
        Assert(bDirty);

        ClearSelection();
        ClearTree();
        
        Context.RebuildTreeFunction(this);

        bDirty = false;
    }

    void FTileViewWidget::DrawItem(FTileViewItem* ItemToDraw, FTileViewContext Context)
    {
    }

    void FTileViewWidget::SetSelection(FTileViewItem* Item)
    {
    }

    void FTileViewWidget::ClearSelection()
    {
    }
}
