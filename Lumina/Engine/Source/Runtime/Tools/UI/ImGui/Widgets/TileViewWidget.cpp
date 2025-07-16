#include "TileViewWidget.h"

#include "Assets/AssetPath.h"

namespace Lumina
{
    void FTileViewWidget::Draw(FTileViewContext Context)
    {
        if (bDirty)
        {
            RebuildTree(Context);
            return;
        }
    
        float paneWidth = ImGui::GetContentRegionAvail().x;
        constexpr float ButtonSize = 125.0f;
        float cellSize = ButtonSize + ImGui::GetStyle().ItemSpacing.x;
        int itemsPerRow = std::max(1, int(paneWidth / cellSize));

        int itemIndex = 0;
        for (FTileViewItem* Item : ListItems)
        {
            const char* DisplayName = Item->GetDisplayName().c_str();

            if (itemIndex % itemsPerRow != 0)
                ImGui::SameLine(0.0f, 20.0f);

            ImGui::PushID(Item);
            ImGui::BeginGroup();

            DrawItem(Item, Context);

            // --- Centered Wrapped Text Drawing ---
            ImFont* font = ImGui::GetIO().Fonts->Fonts[3];
            ImGui::PushFont(font);

            float wrapWidth = ButtonSize;

            // Estimate text height with wrapping
            ImVec2 textSize = ImGui::CalcTextSize(DisplayName, nullptr, true, wrapWidth);
            float reservedHeight = textSize.y;

            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            float textStartX = cursorPos.x + (cellSize - wrapWidth) * 0.5f;

            // Reserve vertical space
            ImGui::Dummy(ImVec2(cellSize, reservedHeight));

            // Draw centered wrapped text
            ImGui::GetWindowDrawList()->AddText(font, font->LegacySize,
                ImVec2(textStartX, cursorPos.y),
                ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]),
                DisplayName, nullptr, wrapWidth);

            ImGui::PopFont();

            ImGui::EndGroup();
            ImGui::PopID();

            ++itemIndex;
        }
    }
    
    void FTileViewWidget::ClearTree()
    {
        for (FTileViewItem* Item : ListItems)
        {
            Memory::Delete(Item);
        }
        
        ListItems.clear();
    }

    void FTileViewWidget::RebuildTree(FTileViewContext Context, bool bKeepSelections)
    {
        Assert(bDirty)

        TVector<FTileViewItem*> CachedSelections = Selections;
        
        ClearSelection();
        ClearTree();

        if (bKeepSelections)
        {
            for (FTileViewItem* Select : CachedSelections)
            {
                SetSelection(Select, Context);
            }
        }
        
        Context.RebuildTreeFunction(this);

        bDirty = false;
    }

    void FTileViewWidget::DrawItem(FTileViewItem* ItemToDraw, const FTileViewContext& Context)
    {
        if (Context.DrawItemOverrideFunction)
        {
            if (Context.DrawItemOverrideFunction(ItemToDraw))
            {
                SetSelection(ItemToDraw, Context);
            }
        }
        else
        {
            if (ImGui::Button("##", ImVec2(125.0f, 125.0f)))
            {
                SetSelection(ItemToDraw, Context);
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ItemToDraw->HasContextMenu())
        {
            ImGui::OpenPopup("ItemContextMenu");
        }
        
        if (ItemToDraw->HasContextMenu())
        {
            if (ImGui::BeginPopupContextItem("ItemContextMenu"))
            {
                TVector<FTileViewItem*> SelectionsToDraw;
                SelectionsToDraw.push_back(ItemToDraw);
                Context.DrawItemContextMenuFunction(SelectionsToDraw);
                
                ImGui::EndPopup();
            }
        }
    }

    void FTileViewWidget::SetSelection(FTileViewItem* Item, FTileViewContext Context)
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

    void FTileViewWidget::ClearSelection()
    {
        for (FTileViewItem* Item : Selections)
        {
            Assert(Item->bSelected)
            
            Item->bSelected = false;
            Item->OnSelectionStateChanged();    
        }

        Selections.clear();
    }
}
