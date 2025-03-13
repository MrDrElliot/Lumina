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
            {
                ImGui::SameLine(0.0f, 20.0f);
            }
        
            ImGui::PushID(Item);
            ImGui::BeginGroup();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            float TextWidth = ImGui::CalcTextSize(DisplayName).x;
            float TextOffset = ((cellSize) - TextWidth) * 0.5f;

            DrawItem(Item, Context);
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + TextOffset);
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ButtonSize);
            ImGui::TextWrapped("%s", DisplayName);
            ImGui::PopTextWrapPos();
            ImGui::PopFont();
            
            ImGui::PopStyleVar();
            
            ImGui::EndGroup();
            ImGui::PopID();
    
            ++itemIndex;
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
        if (ImGui::Button("", ImVec2(125.0f, 125.0f)))
        {
            SetSelection(ItemToDraw, Context);
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            SetSelection(ItemToDraw, Context);
        }
        
        if (ItemToDraw->HasContextMenu())
        {
            if (ImGui::BeginPopupContextItem("ItemContextMenu"))
            {
                TVector<FTileViewItem*> SelectionsToDraw;
                for (FTileViewItem* Selection : Selections)
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
            Assert(Item->bSelected);
            
            Item->bSelected = false;
            Item->OnSelectionStateChanged();    
        }

        Selections.clear();
    }
}
