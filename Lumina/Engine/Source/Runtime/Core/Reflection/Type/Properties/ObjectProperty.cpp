#include "ObjectProperty.h"

#include "imgui.h"
#include "Core/Object/Object.h"

namespace Lumina
{
    void FObjectProperty::Serialize(FArchive& Ar, void* Value)
    {
        CObject* Object = *(CObject**)Value;
        Ar << Object;
        *(CObject**)Value = Object;

    }

    void FObjectProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        CObject* Object = *(CObject**)Value;
        
        Slot.Serialize(Object);

        *(CObject**)Value = Object;

        if (Object)
        {
            Object->Serialize(Slot);
        }
    }

    void FObjectProperty::DrawProperty(void* ValuePtr)
    {
        CObject* Obj = *(CObject**)ValuePtr;

        ImGui::PushID(this);
        
        // Set the text
        const char* ButtonText = Obj ? Obj->GetName().c_str() : "nullptr";

        ImGui::BeginDisabled(true);
        ImGui::Button(ButtonText);
        ImGui::EndDisabled();

        ImGui::PopID();
    }
}
