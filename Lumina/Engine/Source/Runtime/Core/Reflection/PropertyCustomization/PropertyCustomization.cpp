#include "PropertyCustomization.h"

#include "imgui.h"

namespace Lumina
{
    FPropertyHandle::FPropertyHandle(void* InPropertyPointer, FProperty* InProperty)
        : PropertyPointer(InPropertyPointer)
        , Property(InProperty)
    {
    }

    EPropertyChangeOp IPropertyTypeCustomization::UpdateAndDraw(const TSharedPtr<FPropertyHandle>& Property)
    {
        ImGui::PushID(Property.get());
        HandleExternalUpdate(Property);
        EPropertyChangeOp Result = DrawProperty(Property);
        ImGui::PopID();

        return Result;
    }

    void FPropertyCustomizationRegistry::RegisterPropertyCustomization(const FName& Name, PropertyCustomizationRegisterFn Callback)
    {
        Assert(RegisteredProperties.find(Name) == RegisteredProperties.end())
        RegisteredProperties.emplace(Name, Callback);
    }

    void FPropertyCustomizationRegistry::UnregisterPropertyCustomization(const FName& Name)
    {
        Assert(RegisteredProperties.find(Name) != RegisteredProperties.end())
        RegisteredProperties.erase(Name);
    }

    bool FPropertyCustomizationRegistry::IsTypeRegistered(const FName& Name)
    {
        return RegisteredProperties.find(Name) != RegisteredProperties.end();
    }

    TSharedPtr<IPropertyTypeCustomization> FPropertyCustomizationRegistry::GetPropertyCustomizationForType(const FName& Type)
    {
        if (RegisteredProperties.find(Type) != RegisteredProperties.end())
        {
            return RegisteredProperties[Type]();
        }

        return nullptr;
    }
    
}
