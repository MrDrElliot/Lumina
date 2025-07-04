﻿#include "ArrayProperty.h"
#include "imgui.h"

namespace Lumina
{
    void FArrayProperty::Serialize(FArchive& Ar, void* Value)
    {
        FReflectArrayHelper Helper(this, Value);
        SIZE_T ElementCount = Helper.Num();
        
        if (Ar.IsWriting())
        {
            Ar << ElementCount;
            for (SIZE_T i = 0; i < ElementCount; i++)
            {
                Inner->Serialize(Ar, Helper.GetRawAt(i));
            }
        }
        else
        {
            Ar << ElementCount;
            Helper.Resize(ElementCount);

            for (SIZE_T i = 0; i < ElementCount; ++i)
            {
                Inner->Serialize(Ar, Helper.GetRawAt(i));
            }
        }
        
    }

    void FArrayProperty::SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults)
    {
        FArchive* InnerAr = Slot.GetStructuredArchive()->GetInnerAr();
        FReflectArrayHelper Helper(this, Value);
        SIZE_T ElementCount = Helper.Num();
        
        if (InnerAr->IsWriting())
        {
            Slot.Serialize(ElementCount);
            for (SIZE_T i = 0; i < ElementCount; i++)
            {
                Inner->SerializeItem(Slot, Helper.GetRawAt(i));
            }
        }
        else
        {
            Slot.Serialize(ElementCount);
            Helper.Resize(ElementCount);

            for (SIZE_T i = 0; i < ElementCount; ++i)
            {
                Inner->SerializeItem(Slot, Helper.GetRawAt(i));
            }
        }
        
    }

    void FArrayProperty::DrawProperty(void* Object)
    {
        FReflectArrayHelper Helper(this, Object);
        SIZE_T ElementCount = Helper.Num();

        FString HeaderLabel = Name.c_str();
        if (ImGui::CollapsingHeader(HeaderLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();
            ImGui::PushID(this);

            for (SIZE_T i = 0; i < ElementCount; ++i)
            {
                FString ElementLabel = "Element " + eastl::to_string(i);

                if (ImGui::CollapsingHeader(ElementLabel.c_str(), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    ImGui::PushID(static_cast<int>(i));

                    void* ElementPtr = Helper.GetRawAt(i);
                    ImGui::Indent();
                    Inner->DrawProperty(ElementPtr);
                    ImGui::Unindent();

                    ImGui::PopID();
                }
            }

            ImGui::PopID();
            ImGui::Unindent();
        }
    }
}
