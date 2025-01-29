#pragma once

#include "ContentBrowserWindow.h"
#include "ImGuiAssetImporter.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/Image.h"

namespace Lumina
{
    class ImGui_TextureImporter : public ImGuiAssetImporter
    {
    public:

        void Render(ContentBrowserWindow* ContentBrowser) override
        {
            ImGui::Text("Texture Import Options:");
            ImGui::Separator();

            ImageSpecs.Pixels.clear();
       
            // Name (editable)
            ImGui::InputTextWithHint("Name", "Enter asset name",Name, sizeof(Name));
    
            // Mip Levels (editable)
            ImGui::InputInt("Mip Levels", (int*)&ImageSpecs.MipLevels);
    
            // Image Format (editable)
            const char* formats[] = {"RGBA32_UNORM", "RGBA16_UNORM", "RGBA32_FLOAT", "RGBA16_FLOAT", "R8_UNORM"};
            const char* currentFormat = "RGBA32_UNORM"; // Default format
            for (int i = 0; i < IM_ARRAYSIZE(formats); i++)
            {
                if (ImageSpecs.Format == static_cast<EImageFormat>(i))
                {
                    currentFormat = formats[i];
                    break;
                }
            }
            if (ImGui::BeginCombo("Format", currentFormat))
            {
                for (int i = 0; i < IM_ARRAYSIZE(formats); i++)
                {
                    bool isSelected = (currentFormat == formats[i]);
                    if (ImGui::Selectable(formats[i], isSelected))
                    {
                        ImageSpecs.Format = static_cast<EImageFormat>(i);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
    
            // Filter Mode (not editable directly here, but could affect mipmaps or quality)
            static int filterMode = 0;
            ImGui::Text("Filter Mode:");
            ImGui::RadioButton("Nearest", &filterMode, 0);
            ImGui::RadioButton("Linear", &filterMode, 1);
    
            // Wrap Mode (again, affects texture usage and can be part of the import options)
            static int wrapMode = 0;
            ImGui::Text("Wrap Mode:");
            ImGui::RadioButton("Repeat", &wrapMode, 0);
            ImGui::RadioButton("Clamp to Edge", &wrapMode, 1);
    
            // Sample Count (we assume a fixed sample count for simplicity)
            static int sampleCount = 1; // Just an example for single sample, this could be 1, 4, 16, etc.
            ImGui::InputInt("Sample Count", &sampleCount);
            ImageSpecs.SampleCount = EImageSampleCount::ONE; //:static_cast<EImageSampleCount>(sampleCount);
    
            // Separator before Import/Cancel buttons
            ImGui::Separator();
    
            // Import button
            if (ImGui::Button("Import", ImVec2(120, 0)))
            {
                // Perform import with selected options
                LOG_TRACE("Importing asset with selected options.");
                ImageSpecs.DebugName = Name;
                ImageSpecs.Format = EImageFormat::RGBA32_UNORM;
                AssetRegistry::ImportAsset(Name, &ImageSpecs, ContentBrowser->SelectedFile, ContentBrowser->SelectedDirectory);
                ImGui::CloseCurrentPopup();
                ContentBrowser->SelectedFile.clear();
            }
    
            // Cancel button
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup(); // Close the popup when cancel is pressed
                ContentBrowser->SelectedFile.clear();
            }
        }

    private:
    
        FImageSpecification ImageSpecs = FImageSpecification::Default();

    
    };
}