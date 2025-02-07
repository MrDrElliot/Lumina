/*
#include "ContentBrowserItem.h"

#include "ContentBrowserWindow.h"
#include "imgui.h"
#include "Assets/AssetRegistry/AssetRegistry.h"

namespace Lumina
{
    ContentBrowserItem::ContentBrowserItem(const std::filesystem::path& InPath)
    {
        Path = InPath;
    }

    ContentBrowserItem::~ContentBrowserItem()
    {
        
    }

    void ContentBrowserItem::OnRender(TRefPtr<ContentBrowserWindow> ContentBrowser)
    {
        constexpr float buttonSize = 100.0f;
    
        if (bFolder)
        {
            if (ImGui::ImageButton(ContentBrowser->ImFolderTexture, ImVec2(buttonSize, buttonSize), ImVec2(0, 1), ImVec2(1, 0)))
            {
                ContentBrowser->SelectedDirectory = Path.string();
                ContentBrowser->RefreshItems();
                return;
            }
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("ContextMenu");
            }

            if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey_Delete))
            {
                std::filesystem::remove(Path);
                ContentBrowser->RefreshItems();
                return;
            }
            
            if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey_F2))
            {
                bIsRenaming = true;
            }
            
            if (ImGui::BeginPopup("ContextMenu"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    std::filesystem::remove(Path);
                    ContentBrowser->RefreshItems();
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    return;
                    
                }
                ImGui::EndPopup();
            }
            
            if (bIsRenaming)
            {
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputTextWithHint("##Rename", Path.filename().string().c_str(), buffer, sizeof(buffer));
                ImGui::SetKeyboardFocusHere(-1);
                
                if (ImGui::IsKeyReleased(ImGuiKey_Enter))
                {
                    std::string newName = buffer;
                    std::filesystem::path newPath = Path.parent_path() / newName;
            
                    if (!newName.empty() && newPath != Path)
                    {
                        std::filesystem::rename(Path, newPath);
                        Path = newPath;
                        ContentBrowser->RefreshItems();
                    }
            
                    bIsRenaming = false;
                }
            
                if (ImGui::IsKeyReleased(ImGuiKey_Escape) || ImGui::IsMouseClicked(0))
                {
                    bIsRenaming = false;
                }
            }
            else
            {
                float textWidth = ImGui::CalcTextSize(Path.filename().stem().string().c_str()).x - 10.0f;
                float textOffset = (buttonSize - textWidth) * 0.5f;
            
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize - textOffset);
                
                ImGui::TextWrapped("%s", Path.filename().string().c_str());
                
                ImGui::PopTextWrapPos();
            }
            
    
        }
        else
        {
            if (ImGui::ImageButton(ContentBrowser->ImAssetTexture, ImVec2(buttonSize, buttonSize), ImVec2(0, 1), ImVec2(1, 0)))
            {

            }

            if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey_Delete))
            {
               // AssetRegistry::Get()->NotifyAssetDeleted(AssetMetadata);
                std::filesystem::remove(Path);
                ContentBrowser->RefreshItems();
                return;
            }
            
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::BeginTooltip();
                
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
    
                // Header Section
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Asset: %s", Path.stem().string().c_str());
                ImGui::Separator();
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
                ImGui::Text("Properties");
                ImGui::PopFont();
    
                // Display the asset size
                ImGui::Text("Size: %ld bytes", std::filesystem::file_size(Path));
    
                // Display metadata details
                /*ImGui::Text("Name: %s", AssetMetadata.Name.c_str());
                ImGui::Text("Type: %s", AssetTypeToString(AssetMetadata.AssetType).c_str());
                ImGui::Text("Path: %s", AssetMetadata.Path.c_str());
                ImGui::Text("Origin Path: %s", AssetMetadata.OriginPath.c_str());#1#
            
                ImGui::Separator();
    
                // Action buttons
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
                
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(2);

                ImGui::EndTooltip();
            }

        
            float textWidth = ImGui::CalcTextSize(Path.filename().stem().string().c_str()).x - 10.0f;
            float textOffset = (buttonSize - textWidth) * 0.5f;
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize - textOffset);
            ImGui::TextWrapped("%s", Path.filename().stem().string().c_str());
            ImGui::PopTextWrapPos();
    
        }
    }
}
*/
