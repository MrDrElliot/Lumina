
#include "ObjectCore.h"
#include "Class.h"
#include "Object.h"
#include "Core/Math/Math.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    /** Allocates a section of memory for the new object, does not place anything into the memory */
    CObjectBase* AllocateObject(const CClass* InClass, FName InName, EObjectFlags InFlags)
    {
        // Force 16-byte minimal alignment for cache friendliness.
        uint32 Alignment = Math::Max<uint32>(16, InClass->GetAlignment());
        
        return (CObjectBase*)FMemory::Malloc(InClass->GetSize(), Alignment);
    }

    
    FName MakeUniqueObjectName(const CClass* Class, FName InBaseName)
    {
        // Default the base name to the class name if none is provided
        const FName BaseName = (InBaseName == NAME_None) ? Class->GetName() : InBaseName;

        FName TestName = BaseName;
        CObject* FoundObj = nullptr;
        do
        {
            int32_t Index = ++Class->ClassUnique;
            if (Index > 1)
            {
                FString String = BaseName.ToString() + "_" + eastl::to_string(Index);
                TestName = FName(String);
            }
            FoundObj = FindObjectFast(Class, TestName);
        }
        while (FoundObj != nullptr);

        return TestName;
    }


    CObject* StaticAllocateObject(const FConstructCObjectParams& Params)
    {
        const CClass* Class = Params.Class;
        FName Name = MakeUniqueObjectName(Class, Params.Name);
        EObjectFlags Flags = Params.Flags;
        
        CObjectBase* Object = AllocateObject(Class, Name, Flags);

        FMemory::MemsetZero(Object, Params.Class->GetSize());
        new (Object) CObjectBase(const_cast<CClass*>(Params.Class), Params.Flags, Params.Name);

        CObject* Obj = (CObject*)Object;
        Params.Class->ClassConstructor(FObjectInitializer(Obj, Params));

        Obj->PostInitProperties();
        
        return Obj;
    }
    
    CObject* FindObjectFast(const CClass* InClass, FName ObjectName)
    {
        if (ObjectNameHash.find(ObjectName) != ObjectNameHash.end())
        {
            return (CObject*)ObjectNameHash.at(ObjectName);
        }

        return nullptr;
    }

    CObject* StaticLoadObject(const CClass* InClass, const TCHAR* InPath, const TCHAR* InName)
    {
        CObject* FoundObject = nullptr;
        FoundObject = FindObjectFast(InClass, InName);
        if (FoundObject != nullptr)
        {
            return FoundObject;
        }

        

        return nullptr;

    }

    template<typename TPropertyType>
    TPropertyType* NewProperty(FFieldOwner Owner, const FPropertyParams* Param)
    {
        TPropertyType* Type = FMemory::New<TPropertyType>(Owner);
        Type->Name = Param->Name;
        Type->Offset = Param->Offset;
        Type->Owner = Owner;

        return Type;
    }

    void ConstructProperties(CStruct* Owner, const FPropertyParams* const* Properties, uint32 NumProperties)
    {
        for (uint32 i = 0; i < NumProperties; ++i)
        {
            FFieldOwner FieldOwner;
            FieldOwner.Variant.emplace<CStruct*>(Owner);
    
            const FPropertyParams* Param = Properties[i];
            FProperty* NewProp = nullptr;
    
            switch (Param->TypeFlags)
            {
            case EPropertyTypeFlags::Int8:
                NewProp = NewProperty<FInt8Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Int16:
                NewProp = NewProperty<FInt16Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Int32:
                NewProp = NewProperty<FInt32Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Int64:
                NewProp = NewProperty<FInt64Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::UInt8:
                NewProp = NewProperty<FUInt8Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::UInt16:
                NewProp = NewProperty<FUInt16Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::UInt32:
                NewProp = NewProperty<FUInt32Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::UInt64:
                NewProp = NewProperty<FUInt64Property>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Float:
                NewProp = NewProperty<FFloatProperty>(FieldOwner, Param);
                break;
            case EPropertyTypeFlags::Double:
                NewProp = NewProperty<FDoubleProperty>(FieldOwner, Param);
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
            default:
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

        // Link this class to it's parent. (if it has one).
        FinalClass->Link();
    }

    void ConstructCEnum(CEnum** OutEnum, const FEnumParams& Params)
    {
        FConstructCObjectParams ObjectParms(CEnum::StaticClass());
        ObjectParms.Name = Params.Name;
        ObjectParms.Flags = OF_None;

        CEnum* NewEnum = (CEnum*)StaticAllocateObject(ObjectParms);
        *OutEnum = NewEnum;

        for (size_t i = 0; i < Params.NumParams; i++)
        {
            const FEnumeratorParam Param = Params.Params[i];
            NewEnum->AddEnum(Param.NameUTF8, Param.Value);
        }
    }
}
