#include "ObjectProperty.h"

#include "imgui.h"
#include "Core/Object/Object.h"

namespace Lumina
{
    void FObjectProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        FArchive* InnerAr = Slot.GetStructuredArchive()->GetInnerAr();
        
        CObject* Object = *(CObject**)Value;
        if (InnerAr->IsWriting() && Object == nullptr)
        {
            return;
        }
        
        Slot.Serialize(Object);

        *(CObject**)Value = Object;

        if (Object)
        {
            Object->Serialize(Slot);
            Object->Serialize(*InnerAr);
        }

    }

    void FObjectProperty::DrawProperty(void* Object)
    {
        CObject* Obj = *(CObject**)Object;

        ImGui::PushID(this);
        
        // Set the text
        const char* ButtonText = Obj ? Obj->GetName().c_str() : "nullptr";

        ImGui::BeginDisabled(true);
        ImGui::Button(ButtonText);
        ImGui::EndDisabled();

        ImGui::PopID();
    }
}
