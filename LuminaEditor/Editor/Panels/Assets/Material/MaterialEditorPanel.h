#pragma once

#include "Panels/ImGuiWindow.h"
#include "Renderer/Material.h"
#include <imgui.h>

namespace Lumina
{
	class FMaterialEditorPanel
	{
	public:

        static void Render(FMaterialAttributes& Attributes)
        {
            if (ImGui::Begin("Material Editor"))
            {
                // Title with styling
                ImGui::Text("Material Properties");
                ImGui::Separator();

                // Base Color (RGB)
                ImGui::ColorEdit4("Base Color", &Attributes.BaseColor[0], ImGuiColorEditFlags_NoInputs);

                // Roughness
                ImGui::SliderFloat("Roughness", &Attributes.Roughness, 0.0f, 1.0f, "Roughness: %.2f");

                // Metallic
                ImGui::SliderFloat("Metallic", &Attributes.Metallic, 0.0f, 1.0f, "Metallic: %.2f");

                // Emissive Intensity
                ImGui::SliderFloat("Emissive Intensity", &Attributes.EmissiveIntensity, 0.0f, 10.0f, "Intensity: %.2f");

                ImGui::End();
            }
        }
	};
}