#include "Class.h"

#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    
    LUMINA_API void AllocateStaticClass(const TCHAR* Name, CClass** OutClass, uint32 Size, uint32 Alignment)
    {
        Assert(*OutClass == nullptr);

        CClass* NewClass = FMemory::New<CClass>(FName(StringUtils::FromWideString(Name)), Size, Alignment, EObjectFlags::None);
        NewClass->BeginRegister();

        *OutClass = NewClass;
    }

    void CStruct::AddField(FProperty* Property)
    {
        if (LinkedProperty == nullptr)
        {
            LinkedProperty = Property;
        }
        else
        {
            FProperty* Current = LinkedProperty;
            while (Current->Next != nullptr)
            {
                Current = (FProperty*)Current->Next;
            }
            Current->Next = Property;
        }
        Property->Next = nullptr;
    }

    FProperty* CStruct::GetProperty(const FString& Name)
    {
        FProperty* Current = LinkedProperty;
        while (Current != nullptr)
        {
            if (Current->Name == Name)
            {
                return Current;
            }
            
            Current = (FProperty*)Current->Next;
        }
        return nullptr;
    }
}
