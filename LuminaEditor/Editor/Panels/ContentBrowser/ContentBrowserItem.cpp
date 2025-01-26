#include "ContentBrowserItem.h"

#include "ContentBrowserWindow.h"
#include "imgui.h"
#include "Assets/AssetRegistry/AssetRegistry.h"

namespace Lumina
{
    ContentBrowserItem::ContentBrowserItem(const std::filesystem::path& InPath, const FAssetMetadata& Metadata, bool bIsFolder)
    {
        Path = InPath;
        AssetMetadata = Metadata;
        bFolder = bIsFolder;
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
            }
    
            float textWidth = ImGui::CalcTextSize(Path.filename().stem().string().c_str()).x - 10.0f;
            float textOffset = (buttonSize - textWidth) * 0.5f;
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize - textOffset);
            ImGui::TextWrapped("%s", Path.filename().string().c_str());
            ImGui::PopTextWrapPos();
    
        }
        else
        {
    
            // Display the .lum file as a button
            if (ImGui::ImageButton(ContentBrowser->ImAssetTexture, ImVec2(buttonSize, buttonSize), ImVec2(0, 1), ImVec2(1, 0)))
            {
                ImGui::OpenPopup("lum_context_menu");
            }
            ImGui::SetItemTooltip("Click to open context menu for the asset.");
        
            float textWidth = ImGui::CalcTextSize(Path.filename().stem().string().c_str()).x - 10.0f;
            float textOffset = (buttonSize - textWidth) * 0.5f;
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize - textOffset);
            ImGui::TextWrapped("%s", Path.filename().stem().string().c_str());
            ImGui::PopTextWrapPos();
    
    
            // Define the context menu
            if ( ImGui::BeginPopup("lum_context_menu"))
            {
                // Add some padding around the context menu content
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10)); // Item spacing
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5)); // Padding around items
    
                // Header Section
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Asset: %s", Path.stem().string().c_str());
                ImGui::Separator();
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
                ImGui::Text("Properties");
                ImGui::PopFont();
    
                // Display the asset size
                ImGui::Text("Size: %ld bytes", std::filesystem::file_size(Path));
    
                // Display metadata details
                ImGui::Text("Name: %s", AssetMetadata.Name.c_str());
                ImGui::Text("GUID: %s", AssetMetadata.Guid.ToString().c_str());
                ImGui::Text("Path: %s", AssetMetadata.Path.c_str());
                ImGui::Text("Origin Path: %s", AssetMetadata.OriginPath.c_str());
                ImGui::Text("Type: %s", AssetTypeToString(AssetMetadata.AssetType).c_str());
            
                ImGui::Separator();
    
                // Action buttons
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // Light red color
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Light red hover color
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f)); // Light red active color
    
                if (ImGui::Button("Open"))
                {
                    // Handle open logic
                    ImGui::CloseCurrentPopup();
                }
    
                if (ImGui::Button("Delete"))
                {
                    // Handle delete logic
                    std::filesystem::remove(Path);
                    ImGui::CloseCurrentPopup();
                }
    
                ImGui::PopStyleColor(3); // Pop the button color styles
    
                ImGui::PopStyleVar(2); // Pop item spacing and frame padding styles
    
                ImGui::EndPopup();
            }
        }
    }
}
