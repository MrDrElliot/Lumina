#include "MaterialOutput.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
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
                    ImGui::SetNextItemWidth(100.0f);
                    ImGui::DragFloat("##Value", (float*)NodeValue);
                    ReturnSize = 100.0f;
                }
                break;
            case EMaterialInputType::Float2:
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::DragFloat2("##Value", (float*)NodeValue);
                    ReturnSize = 150.0f;
                }
                break;
            case EMaterialInputType::Float3:
                {
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::ColorEdit3("##Value", (float*)NodeValue);
                    ReturnSize = 150.0f;
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
                    CObject* TextureValue = static_cast<CObject*>(NodeValue);

                    ImGui::SetNextItemWidth(200.0f);

                    if (TextureValue != nullptr)
                    {
                        FRHIImageRef Image = Cast<CTexture>(TextureValue)->RHIImage;
                        if (Image)
                        {
                            ImTextureRef ImText = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(Image);
                            ImGui::Image(ImText, {164.0f, 164.0f});
                        }
                    }

                    if (ImGui::Button("Choose Texture"))
                    {
                        ImGui::OpenPopup("ObjectSelectorPopup");
                    }

                    ImGui::SetNextWindowSize({ 300.0f, 300.0f });

                    if (ImGui::BeginPopup("ObjectSelectorPopup"))
                    {
                        FARFilter Filter;
                        Filter.ClassNames.push_back("CTexture");

                        ImGuiX::ObjectSelector(Filter, TextureValue);

                        if (TextureValue != MaterialNode->GetNodeDefaultValue())
                        {
                            MaterialNode->SetNodeValue(TextureValue);
                        }

                        ImGui::EndPopup();
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
