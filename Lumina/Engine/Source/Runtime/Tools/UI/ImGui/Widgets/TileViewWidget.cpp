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
    
        float PaneWidth = ImGui::GetContentRegionAvail().x;
        constexpr float ThumbnailSize = 125.0f;
        float CellSize = ThumbnailSize + ImGui::GetStyle().ItemSpacing.x;
        int ItemsPerRow = std::max(1, int(PaneWidth / CellSize));

        int ItemIndex = 0;
        for (FTileViewItem* Item : ListItems)
        {
            const char* DisplayName = Item->GetDisplayName().c_str();

            if (ItemIndex % ItemsPerRow != 0)
            {
                ImGui::SameLine(0.0f, 15.0f);
            }

            ImGui::PushID(Item);
            ImGui::BeginGroup();

            DrawItem(Item, Context);

            ImFont* Font = ImGui::GetIO().Fonts->Fonts[3];
            ImGui::PushFont(Font);

            float WrapWidth = ThumbnailSize;

            ImVec2 TextSize = ImGui::CalcTextSize(DisplayName, nullptr, true, WrapWidth);
            float ReservedHeight = TextSize.y;

            ImVec2 CursorPos = ImGui::GetCursorScreenPos();
            float TextStartX = CursorPos.x + (CellSize - WrapWidth) * 0.5f;

            ImGui::Dummy(ImVec2(CellSize, ReservedHeight));

            ImGui::GetWindowDrawList()->AddText(Font, Font->LegacySize,
                ImVec2(TextStartX, CursorPos.y),
                ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]),
                DisplayName, nullptr, WrapWidth);

            ImGui::PopFont();

            ImGui::EndGroup();
            ImGui::PopID();

            ++ItemIndex;
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
