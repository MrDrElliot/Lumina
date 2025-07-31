#pragma once
#include "Core/Functional/Function.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{
    class FProperty;

    class LUMINA_API FPropertyHandle
    {
    public:

        FPropertyHandle(void* InPropertyPointer, FProperty* InProperty);

        template<typename T>
        T* GetTypePropertyPtr()
        {
            return (T*)PropertyPointer;
        }
        
        void* PropertyPointer;
        FProperty* Property;
    };

    enum class LUMINA_API EPropertyChangeOp : uint8
    {
        None,
        Updated,
    };
    
    struct LUMINA_API IPropertyTypeCustomization : TSharedFromThis<IPropertyTypeCustomization>
    {
    public:

        virtual EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) = 0;
        
        EPropertyChangeOp UpdateAndDraw(const TSharedPtr<FPropertyHandle>& Property);

        virtual void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) = 0;

        virtual void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) = 0;
        
    };
    
    class LUMINA_API FPropertyCustomizationRegistry
    {
    public:
        using PropertyCustomizationRegisterFn = TFunction<TSharedPtr<IPropertyTypeCustomization>()>;

        
        void RegisterPropertyCustomization(const FName& Name, PropertyCustomizationRegisterFn Callback);
        void UnregisterPropertyCustomization(const FName& Name);

        bool IsTypeRegistered(const FName& Name);
        
        TSharedPtr<IPropertyTypeCustomization> GetPropertyCustomizationForType(const FName& Type);
    

    private:
        
        THashMap<FName, PropertyCustomizationRegisterFn> RegisteredProperties;
    };
}
