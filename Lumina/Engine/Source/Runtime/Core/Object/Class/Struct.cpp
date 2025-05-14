#include "Core/Object/Class.h"
#include "Core/Object/Field.h"
#include "Core/Reflection/Type/LuminaTypes.h"

IMPLEMENT_INTRINSIC_CLASS(CStruct, CField, LUMINA_API)
namespace Lumina
{

    void CStruct::SetSuperStruct(CStruct* InSuper)
    {
        SuperStruct = InSuper;
    }

    void CStruct::RegisterDependencies()
    {
        if (SuperStruct != nullptr)
        {
            SuperStruct->RegisterDependencies();
        }
    }

    bool CStruct::IsChildOf(const CStruct* Base) const
    {
        // Do *not* call IsChildOf with a nullptr. It is UB.
        Assert(this);
        
        if (Base == nullptr)
        {
            return false;
        }

        bool bOldResult = false;
        for (const CStruct* Temp=this; Temp; Temp=Temp->GetSuperStruct())
        {
            if (Temp == Base)
            {
                bOldResult = true;
                break;
            }
        }

        return bOldResult;
    }

    void CStruct::Link()
    {
        if (SuperStruct != nullptr)
        {
            SuperStruct->Link();
        }

        if (SuperStruct && SuperStruct->LinkedProperty)
        {
            FProperty* SuperProperty = SuperStruct->LinkedProperty;

            if (LinkedProperty == nullptr)
            {
                LinkedProperty = SuperProperty;
            }
            else
            {
                FProperty* Current = LinkedProperty;
                while (Current->Next != nullptr)
                {
                    Current = (FProperty*)Current->Next;
                }
                Current->Next = SuperProperty;
            }
        }
    }

    FProperty* CStruct::GetProperty(const FName& Name)
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

    void CStruct::AddProperty(FProperty* Property)
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
}
