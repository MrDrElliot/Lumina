
#include "ObjectCore.h"
#include "Class.h"
#include "Object.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Engine/Engine.h"
#include "Core/Math/Math.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/ArrayProperty.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"
#include "Core/Reflection/Type/Properties/ObjectProperty.h"
#include "Core/Reflection/Type/Properties/StringProperty.h"
#include "Core/Reflection/Type/Properties/StructProperty.h"

namespace Lumina
{
    /** Allocates a section of memory for the new object, does not place anything into the memory */
    CObjectBase* AllocateCObjectMemory(const CClass* InClass, EObjectFlags InFlags)
    {
        // Force 16-byte minimal alignment for cache friendliness.
        uint32 Alignment = Math::Max<uint32>(16, InClass->GetAlignment());
        
        return (CObjectBase*)Memory::Malloc(InClass->GetSize(), Alignment);
    }

    FString GetObjectNameFromPath(const FString& InPath)
    {
        SIZE_T Pos = InPath.find_last_of("/");
        if (Pos != FString::npos)
        {
            return InPath.substr(Pos + 1);
        }

        return InPath;
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


    CObject* StaticAllocateObject(FConstructCObjectParams& Params)
    {
        FName UniqueName = MakeUniqueObjectName(Params.Class, Params.Name);
        Params.Name = UniqueName;
        EObjectFlags Flags = Params.Flags;

        CObjectBase* Object = AllocateCObjectMemory(Params.Class, Flags);

        Memory::MemsetZero(Object, Params.Class->GetSize());
        new (Object) CObjectBase(const_cast<CClass*>(Params.Class), Params.Flags, Params.Package, UniqueName);

        CObject* Obj = (CObject*)Object;
        Params.Class->ClassConstructor(FObjectInitializer(Obj, Params));
        
        Obj->PostInitProperties();
        
        return Obj;
    }
    
    CObject* FindObjectFast(const CClass* InClass, FName QualifiedName)
    {
        CObject* ReturnObject = nullptr;
        
        if (ObjectNameHash.find(QualifiedName) != ObjectNameHash.end())
        {
            ReturnObject = (CObject*)ObjectNameHash.at(QualifiedName);
        }
        
        return ReturnObject;
    }

    CObject* StaticLoadObject(const CClass* InClass, const TCHAR* QualifiedName)
    {
        FString FullPath = WIDE_TO_UTF8(QualifiedName);
        FName ObjectName = FName(GetObjectNameFromPath(FullPath));
        
        CObject* FoundObject = FindObjectFast(InClass, ObjectName);

        if (FoundObject == nullptr)
        {
            FAssetManager* Manager = GEngine->GetEngineSubsystem<FAssetManager>();
            FAssetRequest* Request = Manager->LoadAsset(FullPath);
            Manager->FlushAsyncLoading();

            FoundObject = Request->GetPendingObject();
        }
        
        return FoundObject;
    }

    void ResolveObjectPath(FString& OutPath, const FStringView& InPath)
    {
        
    }

    CObject* NewObject(CClass* InClass, const TCHAR* Package, FName Name, EObjectFlags Flags)
    {
        FConstructCObjectParams Params(InClass);
        Params.Name = Name;
        Params.Flags = Flags;
        Params.Package = Package;

        return StaticAllocateObject(Params);
        
    }

    template<typename TPropertyType>
    TPropertyType* NewFProperty(FFieldOwner Owner, const FPropertyParams* Param)
    {
        TPropertyType* Type = Memory::New<TPropertyType>(Owner, Param);
        Type->Name = Param->Name;
        Type->Offset = Param->Offset;
        Type->Owner = Owner;

        return Type;
    }
    
    
    void ConstructProperties(FFieldOwner FieldOwner, const FPropertyParams* const*& Properties, uint32& NumProperties)
    {
        const FPropertyParams* Param = *--Properties;

        // Indicates the property has an assosicated inner property, which would be next in the Properties list.
        uint32 ReadMore = 0;

        
        FProperty* NewProperty = nullptr;
    
        switch (Param->TypeFlags)
        {
        case EPropertyTypeFlags::Int8:
            NewFProperty<FInt8Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Int16:
            NewFProperty<FInt16Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Int32:
            NewFProperty<FInt32Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Int64:
            NewFProperty<FInt64Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::UInt8:
            NewFProperty<FUInt8Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::UInt16:
            NewFProperty<FUInt16Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::UInt32:
            NewFProperty<FUInt32Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::UInt64:
            NewFProperty<FUInt64Property>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Float:
            NewFProperty<FFloatProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Double:
            NewFProperty<FDoubleProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Bool:
            NewFProperty<FBoolProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Object:
            NewFProperty<FObjectProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Class:
            break;
        case EPropertyTypeFlags::Name:
            NewFProperty<FNameProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::String:
            NewFProperty<FStringProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Struct:
            NewFProperty<FStructProperty>(FieldOwner, Param);
            break;
        case EPropertyTypeFlags::Enum:
            {
                NewProperty = NewFProperty<FEnumProperty>(FieldOwner, Param);
                
                ReadMore = 1;
            }
            break;
        case EPropertyTypeFlags::Vector:
            {
                NewProperty = NewFProperty<FArrayProperty>(FieldOwner, Param);

                ReadMore = 1;
            }
            break;
        default:
            {
                LOG_CRITICAL("Unsupported property type found while creating: {}", Param->Name);
            }
            break;
        }

        --NumProperties;
        
        for (; ReadMore; --ReadMore)
        {
            FFieldOwner Owner;
            Owner.Variant.emplace<FField*>(NewProperty);
            ConstructProperties(Owner, Properties, NumProperties);
        }
    }

    void InitializeAndCreateFProperties(CStruct* Outer, const FPropertyParams* const* PropertyArray, uint32 NumProperties)
    {
        // Move to iterate backwards.
        PropertyArray += NumProperties;
        while (NumProperties)
        {
            FFieldOwner Owner;
            Owner.Variant.emplace<CStruct*>(Outer);
            ConstructProperties(Owner, PropertyArray, NumProperties);
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
        
        InitializeAndCreateFProperties(FinalClass, Params.Params, Params.NumProperties);

        // Link this class to it's parent. (if it has one).
        FinalClass->Link();
    }

    void ConstructCEnum(CEnum** OutEnum, const FEnumParams& Params)
    {
        FConstructCObjectParams ObjectParms(CEnum::StaticClass());
        ObjectParms.Name = Params.Name;
        ObjectParms.Flags = OF_None;
        ObjectParms.Package = CEnum::StaticPackage();

        CEnum* NewEnum = (CEnum*)StaticAllocateObject(ObjectParms);
        *OutEnum = NewEnum;

        for (int16 i = 0; i < Params.NumParams; i++)
        {
            const FEnumeratorParam* Param = &Params.Params[i];
            NewEnum->AddEnum(Param->NameUTF8, Param->Value);
        }
    }

    void ConstructCStruct(CStruct** OutStruct, const FStructParams& Params)
    {
        CStruct* FinalClass = *OutStruct;
        if (FinalClass != nullptr)
        {
            return;
        }

        FinalClass = (CStruct*)Memory::Malloc(Params.SizeOf, Params.AlignOf);
        FinalClass = ::new (FinalClass) CStruct(TEXT(""), FName(Params.Name), Params.SizeOf, Params.AlignOf, EObjectFlags::OF_None);
        *OutStruct = FinalClass;

        CObjectForceRegistration(FinalClass);
        
        InitializeAndCreateFProperties(FinalClass, Params.Params, Params.NumProperties);

        // Link this class to it's parent. (if it has one).
        FinalClass->Link();
    }
}
