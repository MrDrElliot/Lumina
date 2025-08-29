#include "MaterialOutput.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Core/Object/ObjectArray.h"
#include "imgui-node-editor/imgui_node_editor_internal.h"
#include "Nodes/MaterialGraphNode.h"
#include "Renderer/RenderManager.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    float CMaterialOutput::DrawPin()
    {
        float ReturnSize = 1.0f;
        if (ShouldDrawEditor())
        {
            CMaterialGraphNode* MaterialNode = static_cast<CMaterialGraphNode*>(OwningNode);
            void* NodeValue = MaterialNode->GetNodeDefaultValue();
            switch (InputType)
            {
            case EMaterialInputType::Float:
                {
                    ImGui::SetNextItemWidth(60.0f);
                    ImGui::DragFloat("##Value", (float*)NodeValue, 0.01f);
                    ReturnSize = 60.0f;
                }
                break;
            case EMaterialInputType::Float2:
                {
                    ImGui::SetNextItemWidth(120.0f);
                    ImGui::DragFloat2("##Value", (float*)NodeValue, 0.01f);
                    ReturnSize = 120.0f;
                }
                break;
            case EMaterialInputType::Float3:
                {
                    ImGui::SetNextItemWidth(120.0f);
                    ImGui::ColorEdit3("##Value", (float*)NodeValue);
                    ReturnSize = 120.0f;
                }
                break;
            case EMaterialInputType::Float4:
                {
                    ImGui::SetNextItemWidth(200.0f);
                    ImGui::ColorEdit4("##Value", (float*)NodeValue);
                    ReturnSize = 200.0f;
                }
                break;
            case EMaterialInputType::Texture:
                {
                    FObjectHandle* TextureValue = static_cast<FObjectHandle*>(NodeValue);
                    CTexture* Texture = (CTexture*)TextureValue->Resolve();
                    
                    ImGui::SetNextItemWidth(200.0f);

                    if (Texture != nullptr)
                    {
                        if (FRHIImageRef& Image = Texture->RHIImage)
                        {
                            ImTextureRef ImText = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(Image);
                            ImGui::Image(ImText, ImVec2(164.0f, 164.0f));
                        }
                    }
                    
                    ReturnSize = 200.0f;
                }
                break;
            }
        }
        else
        {
            ImGui::Dummy(ImVec2(1.5f, 1.5f));
        }

        return ReturnSize;
    }
}
