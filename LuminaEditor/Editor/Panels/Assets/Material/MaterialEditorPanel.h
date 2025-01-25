#pragma once

#include <imgui.h>

#include "Renderer/RenderTypes.h"

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
            	
                ImGui::End();
            }
        }
	};
}
