#include "ObjectCore.h"

#include "Class.h"
#include "Object.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{

    template<typename TPropertyType>
    TPropertyType* NewProperty(FFieldOwner Owner, const FPropertyParams* Param)
    {
        TPropertyType* Type = nullptr;
        Type = FMemory::New<TPropertyType>(Owner);

        return Type;
        
    }

    void ConstructProperties(CObject* Owner, const FPropertyParams* const* Properties, uint32 NumProperties)
    {
        for (int i = 0; i < NumProperties; ++i)
        {
            FFieldOwner FieldOwner;
            FieldOwner.Owner.emplace<CObject*>(Owner);
            
            const FPropertyParams* Param =  Properties[i];
            FProperty* NewProp = nullptr;
            
            switch (Param->TypeFlags)
            {
            case EPropertyTypeFlags::Int8:
                NewProp = NewProperty<FUInt8Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Int16:
                break;
            case EPropertyTypeFlags::Int32:
                break;
            case EPropertyTypeFlags::Int64:
                break;
            case EPropertyTypeFlags::UInt8:
                break;
            case EPropertyTypeFlags::UInt16:
                break;
            case EPropertyTypeFlags::UInt32:
                break;
            case EPropertyTypeFlags::UInt64:
                break;
            case EPropertyTypeFlags::Float:
                break;
            case EPropertyTypeFlags::Double:
                break;
            case EPropertyTypeFlags::Bool:
                break;
            case EPropertyTypeFlags::Object:
                break;
            case EPropertyTypeFlags::Class:
                break;
            case EPropertyTypeFlags::Name:
                break;
            case EPropertyTypeFlags::String:
                break;
            }
        }
    }
    
    void ConstructCClass(CClass** OutClass, const FClassParams& Params)
    {
        CClass* FinalClass = *OutClass;
        if (FinalClass != nullptr)
        {
            return;
        }

        FinalClass = Params.RegisterFunc();
        *OutClass = FinalClass;

        CObjectForceRegistration(FinalClass);

        ConstructProperties(FinalClass, Params.Params, Params.NumProperties);
        
    }
}
