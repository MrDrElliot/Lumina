#include "CoreTypeCustomization.h"
#include "imgui.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"

namespace Lumina
{
    void FEnumPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        void* ValuePtr = Property->PropertyPointer;
        FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(Property->Property);

        int64 EnumValue = EnumProperty->GetInnerProperty()->GetSignedIntPropertyValue(ValuePtr);
        int64 CurrentIndex = EnumValue;
        
        SIZE_T EnumCount = EnumProperty->GetEnum()->Names.size();

        const char* PreviewValue = EnumProperty->GetEnum()->GetNameAtValue(CurrentIndex).c_str();

        if (ImGui::BeginCombo("##", PreviewValue))
        {
            for (SIZE_T i = 0; i < EnumCount; ++i)
            {
                const char* Label = EnumProperty->GetEnum()->GetNameAtValue(i).c_str();
                bool IsSelected = (i == CurrentIndex);

                if (ImGui::Selectable(Label, IsSelected))
                {
                    EnumProperty->GetInnerProperty()->SetIntPropertyValue(ValuePtr, i);
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
    }
}
