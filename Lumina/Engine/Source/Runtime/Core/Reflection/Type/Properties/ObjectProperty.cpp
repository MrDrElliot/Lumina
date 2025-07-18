#include "ObjectProperty.h"

#include "imgui.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Object/Object.h"
#include "Tools/UI/ImGui/ImGuiX.h"

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
        CObject*& Obj = *(CObject**)ValuePtr;
        
        float ButtonWidth = ImGui::GetContentRegionAvail().x;

        const char* Label = Obj ? Obj->GetName().c_str() : "nullptr";

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImVec4 BgColor = Obj ? ImVec4(0.2f, 0.25f, 0.3f, 1.0f) : ImVec4(0.3f, 0.1f, 0.1f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, BgColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(BgColor.x + 0.1f, BgColor.y + 0.1f, BgColor.z + 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, BgColor);

        if (ImGui::Button(Label, ImVec2(ButtonWidth, 0)))
        {
            ImGui::OpenPopup("ObjectSelectorPopup");
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        if (ImGui::BeginPopup("ObjectSelectorPopup"))
        {
            CObject* Selected = nullptr;
            FARFilter Filter;
            Filter.ClassNames.push_back("CMaterial");
            ImGuiX::ObjectSelector(Filter, Selected);

            if (Selected)
            {
                *(CObject**)ValuePtr = Selected;
            }
            
            ImGui::EndPopup();
        }
    }
}
