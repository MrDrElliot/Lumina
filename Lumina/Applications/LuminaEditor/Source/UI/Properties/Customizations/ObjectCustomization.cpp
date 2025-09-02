#include "CoreTypeCustomization.h"
#include "imgui.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Material/MaterialInstance.h"
#include "assets/assettypes/mesh/staticmesh/staticmesh.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Reflection/Type/Properties/ObjectProperty.h"
#include "Renderer/RenderManager.h"
#include "Tools/UI/UITextureCache.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "UI/EditorUI.h"

namespace Lumina
{
    static constexpr ImVec2 GButtonSize(30, 0);

    EPropertyChangeOp FCObjectPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(Property->Property);
        
        CObject* Obj = ObjectHandle.Resolve();
        bool bWasChanged = false;

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        ImGui::PushID(this);
        if (ImGui::BeginChild("OP", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            const auto& Style = ImGui::GetStyle();

            const char* Label = Obj ? Obj->GetName().c_str() : "<None>";
            ImGui::BeginDisabled(Obj == nullptr);

            // Temporary stuff.
            ImTextureRef ButtonTexture = FUITextureCache::Get().GetImTexture(Paths::GetEngineResourceDirectory() + "/Textures/SkeletalMeshIcon.png");
            
            ImGui::ImageButton(Label, ButtonTexture, ImVec2(64, 64));
            ImGui::EndDisabled();

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                
            }

            ImGui::SameLine();

            ImGui::BeginGroup();

            float const ComboArrowWidth = ImGui::GetFrameHeight();
            float const TotalPathWidgetWidth = ImGui::GetContentRegionAvail().x;
            float const TextWidgetWidth = TotalPathWidgetWidth - ComboArrowWidth;

            ImGui::SetNextItemWidth(TextWidgetWidth);
            
            const bool bHasObject = Obj != nullptr;

            FString PathString = bHasObject ? Obj->GetPathName() : FString("<None>");
            char PathBuffer[512];
            strncpy(PathBuffer, PathString.c_str(), sizeof(PathBuffer) - 1);
            PathBuffer[sizeof(PathBuffer) - 1] = '\0';
        
            ImGui::PushStyleColor(ImGuiCol_Text, bHasObject ? ImVec4(0.6f, 0.6f, 0.6f, 1.0f) : ImVec4(1.0f, 0.19f, 0.19f, 1.0f));
        
            ImGui::InputText("##ObjectPathText", PathBuffer, sizeof(PathBuffer), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
        
            ImGuiX::ItemTooltip(PathString.c_str());
        
            ImGui::PopStyleColor();

            const ImVec2 ComboDropDownSize(Math::Max(TotalPathWidgetWidth, 500.0f), ImGui::GetFrameHeight() * 20);
            ImGui::SetNextWindowSizeConstraints(ImVec2(ComboDropDownSize.x, 0), ComboDropDownSize);

            ImGui::SameLine(0, 0);
        
            bool bComboOpen = ImGui::BeginCombo("##DataPath", "", ImGuiComboFlags_HeightLarge | ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoPreview);

            if (bComboOpen)
            {
                const float CursorPosYPostFilter = ImGui::GetCursorPosY();
                const float FilterHeight = CursorPosYPostFilter;

                const ImVec2 PreviousCursorPos = ImGui::GetCursorPos();
                const ImVec2 ChildSize(ImGui::GetContentRegionAvail().x, ComboDropDownSize.y - FilterHeight - Style.ItemSpacing.y - Style.WindowPadding.y);
                ImGui::Dummy(ChildSize);
                ImGui::SetCursorPos(PreviousCursorPos);

                SearchFilter.Draw("##Search", ImGui::GetContentRegionAvail().x);
                
                if (ImGui::BeginChild("##OptList", ChildSize, false, ImGuiChildFlags_NavFlattened))
                {
                    FARFilter Filter;
                    Filter.ClassNames.push_back(ObjectProperty->GetPropertyClass()->GetName().ToString());

                    const FAssetDataMap& FilteredAssets = GEngine->GetEngineSubsystem<FAssetRegistry>()->GetAssets();
                    for (const FAssetData* Asset : FilteredAssets)
                    {
                        if (!SearchFilter.PassFilter(Asset->AssetName.c_str()))
                        {
                            continue;
                        }
                        
                        if (ImGui::Selectable(Asset->FullPath.c_str()))
                        {
                            Obj = LoadObject<CObject>(nullptr, Asset->FullPath);
                            ObjectHandle = GObjectArray.ToHandle(Obj);
                            ImGui::CloseCurrentPopup();
                    
                            bWasChanged = true;
                        }
                    }
                }
                
                ImGui::EndChild();
                ImGui::EndCombo();
            }
        
            ImGui::BeginDisabled(Obj == nullptr);
            if (ImGui::Button(LE_ICON_CONTENT_COPY "##Copy", GButtonSize))
            {
                ImGui::SetClipboardText(Obj->GetPathName().c_str());
            }

            ImGui::SameLine();

            if (ImGui::Button(LE_ICON_COG "##Options", GButtonSize))
            {
                
            }

            ImGui::SameLine();
        
            if (ImGui::Button(LE_ICON_CLOSE_CIRCLE "##Clear", GButtonSize))
            {
                ObjectHandle = FObjectHandle();
                bWasChanged = true;
            }
        
            ImGui::EndDisabled();
            ImGui::EndGroup();
        }
        ImGui::EndChild();

        ImGui::PopID();

        ImGui::PopItemWidth();
        
        return bWasChanged ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    void FCObjectPropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        Property->Property->SetValue(Property->ContainerPtr, ObjectHandle, Property->Index);
    }

    void FCObjectPropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        Property->Property->GetValue(Property->ContainerPtr, &ObjectHandle, Property->Index);
    }
}
