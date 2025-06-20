#include "EnumProperty.h"

#include "imgui.h"

namespace Lumina
{
    FEnumProperty::~FEnumProperty()
    {
        Memory::Delete(InnerProperty);
    }

    void FEnumProperty::SetEnum(CEnum* InEnum)
    {
        Enum = InEnum;
    }

    void FEnumProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        FArchive* InnerAr = Slot.GetStructuredArchive()->GetInnerAr();
        
        if (InnerAr->IsReading())
        {
            FName EnumName;
            Slot.Serialize(EnumName);
            
            int64 EnumValue = (int64)Enum->GetEnumValueByName(EnumName);
            InnerProperty->SetIntPropertyValue(Value, EnumValue);

        }
        else
        {
            const int64 IntValue = InnerProperty->GetSignedIntPropertyValue(Value);
            FName EnumName = Enum->GetNameAtValue(IntValue);
            Slot.Serialize(EnumName);
        }
    }
    
    void FEnumProperty::DrawProperty(void* Object)
    {
        void* ValuePtr = GetValuePtr<void>(Object);

        int64 EnumValue = InnerProperty->GetSignedIntPropertyValue(ValuePtr);
        int64 CurrentIndex = EnumValue;
        
        SIZE_T EnumCount = Enum->Names.size();

        const char* PreviewValue = Enum->GetNameAtValue(CurrentIndex).c_str();

        if (ImGui::BeginCombo("##", PreviewValue))
        {
            for (SIZE_T i = 0; i < EnumCount; ++i)
            {
                const char* Label = Enum->GetNameAtValue(i).c_str();
                bool IsSelected = (i == CurrentIndex);

                if (ImGui::Selectable(Label, IsSelected))
                {
                    InnerProperty->SetIntPropertyValue(ValuePtr, i);
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
