#include "CoreTypeCustomization.h"
#include "UI/Tools/AssetEditors/TextureEditor/TextureEditorTool.h"


namespace Lumina
{
    EPropertyChangeOp FNamePropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        // We have to modify a copy here because modifying FName::c_str() will directly modify the global value in the name hash.
        FString NameCopy = DisplayValue.ToString();
        
        if (ImGui::InputText("##Name", const_cast<char*>(NameCopy.c_str()), 256))
        {
            DisplayValue = FName(NameCopy);
            return EPropertyChangeOp::Updated;
        }

        ImGui::PopItemWidth();

        return EPropertyChangeOp::None;
    }

    void FNamePropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        Property->Property->SetValue(Property->ContainerPtr, CachedValue, Property->Index);
    }

    void FNamePropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        FName ActualValue;
        Property->Property->GetValue(Property->ContainerPtr, &ActualValue, Property->Index);
        
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }

    EPropertyChangeOp FStringPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        ImGui::InputText("##Name", const_cast<char*>(DisplayValue.c_str()), 256);

        ImGui::PopItemWidth();
        
        return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
    }

    
    void FStringPropertyCustomization::UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property)
    {
        CachedValue = DisplayValue;
        Property->Property->SetValue(Property->ContainerPtr, CachedValue, Property->Index);
    }

    void FStringPropertyCustomization::HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property)
    {
        FString ActualValue;
        Property->Property->GetValue(Property->ContainerPtr, &ActualValue, Property->Index);
        
        if (CachedValue != ActualValue)
        {
            CachedValue = DisplayValue = ActualValue;
        }
    }
}
