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
    void FNumericPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        ImGui::PushID(this);
    
        EPropertyTypeFlags PropertyType = Property->Property->GetType();
    
        int Min = 0;
        int Max = 0;
        double MinD = 0.0;
        double MaxD = 0.0;
    
        void* Ptr = Property->PropertyPointer;
        
        switch (PropertyType)
        {
            case EPropertyTypeFlags::Int8:
                Min = std::numeric_limits<int8_t>::min();
                Max = std::numeric_limits<int8_t>::max();
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::Int16:
                Min = std::numeric_limits<int16_t>::min();
                Max = std::numeric_limits<int16_t>::max();
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::Int32:
                Min = std::numeric_limits<int32_t>::min();
                Max = std::numeric_limits<int32_t>::max();
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::Int64:
                // No native ImGui support for int64, so cast to double
                MinD = static_cast<double>(std::numeric_limits<int64_t>::min());
                MaxD = static_cast<double>(std::numeric_limits<int64_t>::max());
                ImGui::DragScalar("##Value", ImGuiDataType_S64, Ptr, 1.0f, &MinD, &MaxD);
                break;
    
            case EPropertyTypeFlags::UInt8:
                Min = std::numeric_limits<uint8_t>::min();
                Max = std::numeric_limits<uint8_t>::max();
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::UInt16:
                Min = std::numeric_limits<uint16_t>::min();
                Max = std::numeric_limits<uint16_t>::max();
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::UInt32:
                Min = 0;
                Max = static_cast<int>(std::numeric_limits<uint32_t>::max());
                ImGui::DragInt("##Value", (int*)(Ptr), 1,Min, Max);
                break;
            case EPropertyTypeFlags::UInt64:
                MinD = 0.0;
                MaxD = static_cast<double>(std::numeric_limits<uint64_t>::max());
                ImGui::DragScalar("##Value", ImGuiDataType_U64, Ptr, 1.0f, &MinD, &MaxD);
                break;
    
            case EPropertyTypeFlags::Float:
                    MinD = std::numeric_limits<float>::min();
                    MaxD = std::numeric_limits<float>::max();
                ImGui::DragScalar("##Value", ImGuiDataType_Float, Ptr, 0.1f, &MinD, &MaxD);
                break;
    
            case EPropertyTypeFlags::Double:
                MinD = -DBL_MAX;
                MaxD = DBL_MAX;
                ImGui::DragScalar("##Value", ImGuiDataType_Double, Ptr, 0.1f, &MinD, &MaxD);
                break;
    
            default:
                ImGui::TextDisabled("Unsupported type");
                break;
        }
    
        ImGui::PopID();
    }

    void FNamePropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        void* ValuePtr = Property->PropertyPointer;
        FName* Name = static_cast<FName*>(ValuePtr);
            
        if (ImGui::InputText("##Name", const_cast<char*>(Name->c_str()), 256, ImGuiInputTextFlags_EnterReturnsTrue))
        {
                
        }
    }

    void FVec2PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        void* ValuePtr = Property->PropertyPointer;
        glm::vec2* Vec4Ptr = (glm::vec2*)ValuePtr;
            
        ImGui::DragFloat2("##", glm::value_ptr(*Vec4Ptr), 0.1f);
    }
    
    void FVec3PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FStructProperty* StructProperty = static_cast<FStructProperty*>(Property->Property);
        bool bAsColor = StructProperty->Metadata.HasMetadata("Color");
        void* ValuePtr = Property->PropertyPointer;
        glm::vec3* Vec3Ptr = (glm::vec3*)ValuePtr;
    
        if (bAsColor)
        {
            ImGui::ColorEdit3("##", glm::value_ptr(*Vec3Ptr));
        }
        else
        {
            ImGui::DragFloat3("##", glm::value_ptr(*Vec3Ptr), 0.1f);
        }
    }
    void FVec4PropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FStructProperty* StructProperty = static_cast<FStructProperty*>(Property->Property);
        bool bAsColor = StructProperty->Metadata.HasMetadata("Color");
        void* ValuePtr = Property->PropertyPointer;
        glm::vec4* Vec4Ptr = (glm::vec4*)ValuePtr;
    
        if (bAsColor)
        {
            ImGui::ColorEdit4("##", glm::value_ptr(*Vec4Ptr));
        }
        else
        {
            ImGui::DragFloat4("##", glm::value_ptr(*Vec4Ptr), 0.1f);
        }
    }
    
    void FTransformPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FTransform* TransformPtr = Property->GetTypePropertyPtr<FTransform>();

        constexpr float HeaderWidth = 24;
        
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
                if (ImGui::DragFloat3("T", glm::value_ptr(TransformPtr->Location), 0.1f))
                {
                    
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
                glm::vec3 EulerRotation = glm::degrees(glm::eulerAngles(TransformPtr->Rotation));
                if (ImGui::DragFloat3("R", glm::value_ptr(EulerRotation), 0.1f))
                {
                    TransformPtr->SetRotationFromEuler(EulerRotation);
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
                if (ImGui::DragFloat3("S", glm::value_ptr(TransformPtr->Scale), 0.1f))
                {
                    
                }
            }
        
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        
    }
}
