#include "CoreTypeCustomization.h"
#include "imgui.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"

namespace Lumina
{
    EPropertyChangeOp FEnumPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(Property->Property);
        int64 EnumCount = (int64)EnumProperty->GetEnum()->Names.size();
        bool bWasChanged = false;

        const char* PreviewValue = EnumProperty->GetEnum()->GetNameAtValue(CachedValue).c_str();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        
        if (ImGui::BeginCombo("##", PreviewValue))
        {
            for (int64 i = 0; i < EnumCount; ++i)
            {
                const char* Label = EnumProperty->GetEnum()->GetNameAtValue(i).c_str();
                bool IsSelected = (i == CachedValue);

                if (ImGui::Selectable(Label, IsSelected))
                {
                    CachedValue = i;
                    bWasChanged = true;
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();

        return bWasChanged ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    void FEnumPropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(Property->Property);
        EnumProperty->GetInnerProperty()->SetIntPropertyValue(Property->Property->GetValuePtr<void>(Property->ContainerPtr), CachedValue);
        
    }

    void FEnumPropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(Property->Property);
        
        CachedValue = EnumProperty->GetInnerProperty()->GetSignedIntPropertyValue(Property->Property->GetValuePtr<void>(Property->ContainerPtr));
    }
}
