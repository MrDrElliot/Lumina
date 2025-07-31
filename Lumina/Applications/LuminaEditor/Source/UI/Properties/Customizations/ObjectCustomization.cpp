#include "CoreTypeCustomization.h"
#include "imgui.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Material/MaterialInstance.h"
#include "assets/assettypes/mesh/staticmesh/staticmesh.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Reflection/Type/Properties/ObjectProperty.h"
#include "Renderer/RenderManager.h"
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
        
        ImGui::PushID(this);
        if (ImGui::BeginChild("OP", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            const auto& Style = ImGui::GetStyle();

            const char* Label = Obj ? Obj->GetName().c_str() : "<None>";
            ImGui::BeginDisabled(Obj == nullptr);

            // Temporary stuff.
            ImTextureID ButtonTexture = 0;
            CClass* Class = ObjectProperty->GetPropertyClass();
            if (Class == CMaterialInterface::StaticClass())
            {
                ButtonTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::MaterialIcon);
            }
            else if (Class == CTexture::StaticClass())
            {
                ButtonTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::TextureIcon);
            }
            else if (Class == CStaticMesh::StaticClass())
            {
                ButtonTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::StaticMeshIcon);
            }
            else
            {
                ButtonTexture = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(FEditorUI::CorruptIcon);
            }
            
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

                static ImGuiTextFilter SearchFilter;
                if (ImGui::BeginChild("##OptList", ChildSize, false, ImGuiChildFlags_NavFlattened))
                {
                    FARFilter Filter;
                    //Filter.ClassNames.push_back(ObjectProperty->GetPropertyClass()->GetName().ToString());

                    TVector<FAssetData> FilteredAssets;
                    GEngine->GetEngineSubsystem<FAssetRegistry>()->GetAssets(Filter, FilteredAssets);
                    for (const FAssetData& Asset : FilteredAssets)
                    {
                        if (!SearchFilter.PassFilter(Asset.Name.c_str()))
                        {
                            continue;
                        }
                        
                        FString VirtualPath = Paths::ConvertToVirtualPath(Asset.Path);
                        if (ImGui::Selectable(VirtualPath.c_str()))
                        {
                            VirtualPath += "." + Asset.Name.ToString();
                            FName AssetName = VirtualPath.c_str();
                            Obj = LoadObject<CObject>(AssetName);
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

        return bWasChanged ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    void FCObjectPropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        *(FObjectHandle*)Property->PropertyPointer = ObjectHandle;
    }

    void FCObjectPropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        ObjectHandle = *(FObjectHandle*)Property->PropertyPointer;
    }
}
