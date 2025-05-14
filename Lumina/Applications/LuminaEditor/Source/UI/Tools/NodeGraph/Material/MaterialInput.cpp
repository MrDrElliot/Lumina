#include "MaterialInput.h"
#include "Nodes/MaterialGraphNode.h"

namespace Lumina
{
    void CMaterialInput::DrawPin()
    {
        if (ShouldDrawEditor())
        {
            CMaterialGraphNode* MaterialNode = static_cast<CMaterialGraphNode*>(OwningNode);
            float* NodeValue = MaterialNode->GetNodeDefaultValue();
            switch (InputType)
            {
            case EMaterialInputType::Float:
                {
                    ImGui::SetNextItemWidth(100.0f);
                    ImGui::DragFloat("##Value", NodeValue);
                }
                break;
            case EMaterialInputType::Float2:
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::DragFloat2("##Value", NodeValue);
                }
                break;
            case EMaterialInputType::Float3:
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::ColorEdit3("##Value", NodeValue);
                }
                break;
            case EMaterialInputType::Float4:
                {
                    ImGui::SetNextItemWidth(200.0f);
                    ImGui::ColorEdit4("##Value", NodeValue);
                }
                break;
            }
        }
        else
        {
            ImGui::Dummy(ImVec2(1.5f, 1.5f));
        }
    }
}
