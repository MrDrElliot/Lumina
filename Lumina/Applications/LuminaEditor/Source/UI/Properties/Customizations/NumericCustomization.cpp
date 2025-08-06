#include "CoreTypeCustomization.h"
#include "imgui.h"
#include "Core/Object/ObjectCore.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include <limits>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Math/Transform.h"
#include "Core/Reflection/Type/Properties/StructProperty.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{

    EPropertyChangeOp FVec2PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        ImGui::DragFloat2("##", glm::value_ptr(DisplayValue), 0.1f);

        return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    void FVec2PropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        *(glm::vec2*)Property->PropertyPointer = CachedValue;
    }

    void FVec2PropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        glm::vec2& ActualValue = *Property->GetTypePropertyPtr<glm::vec2>();
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }

    EPropertyChangeOp FVec3PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FStructProperty* StructProperty = static_cast<FStructProperty*>(Property->Property);

        if (StructProperty->Metadata.HasMetadata("Color"))
        {
            ImGui::ColorEdit3("##", glm::value_ptr(DisplayValue));
        }
        else
        {
            ImGui::DragFloat3("##", glm::value_ptr(DisplayValue), 0.1f);
        }

        return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }
    
    void FVec3PropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        *(glm::vec3*)Property->PropertyPointer = CachedValue;
    }

    void FVec3PropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        glm::vec3& ActualValue = *Property->GetTypePropertyPtr<glm::vec3>();
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }

    EPropertyChangeOp FVec4PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FStructProperty* StructProperty = static_cast<FStructProperty*>(Property->Property);

        if (StructProperty->Metadata.HasMetadata("Color"))
        {
            ImGui::ColorEdit4("##", glm::value_ptr(DisplayValue));
        }
        else
        {
            ImGui::DragFloat4("##", glm::value_ptr(DisplayValue), 0.1f);
        }

        return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    void FVec4PropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        *(glm::vec4*)Property->PropertyPointer = CachedValue;
    }

    void FVec4PropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        glm::vec4& ActualValue = *Property->GetTypePropertyPtr<glm::vec4>();
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }

    EPropertyChangeOp FTransformPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        constexpr float HeaderWidth = 24;

        bool bWasChanged = false;
        
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
        if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_None))
        {
            ImGui::TableSetupColumn("Header", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, HeaderWidth);
            ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_NoHide);

                    
            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                {
                    ImGui::Text(LE_ICON_AXIS_ARROW);
                    ImGuiX::TextTooltip("Translation");
                }
        
                ImGui::TableNextColumn();
                if (ImGui::DragFloat3("T", glm::value_ptr(DisplayValue.Location), 0.01f))
                {
                    bWasChanged = true;
                }
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                {
                    ImGui::Text(LE_ICON_ROTATE_360);
                    ImGuiX::TextTooltip("Rotation");
                }
        
                ImGui::TableNextColumn();
                glm::vec3 EulerRotation = glm::degrees(glm::eulerAngles(DisplayValue.Rotation));
                if (ImGui::DragFloat3("R", glm::value_ptr(EulerRotation), 0.01f))
                {
                    DisplayValue.SetRotationFromEuler(EulerRotation);
                    bWasChanged = true;
                }
            }
        
            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                {
                    ImGui::Text(LE_ICON_ARROW_TOP_RIGHT_BOTTOM_LEFT);
                    ImGuiX::TextTooltip("Scale");
                }
                
                ImGui::TableNextColumn();
                if (ImGui::DragFloat3("S", glm::value_ptr(DisplayValue.Scale), 0.01f))
                {
                    bWasChanged = true;
                }
            }
        
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        return bWasChanged ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
        
    }

    void FTransformPropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        *(FTransform*)Property->PropertyPointer = CachedValue;
    }

    void FTransformPropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        FTransform& ActualValue = *(FTransform*)Property->PropertyPointer;
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }
}
