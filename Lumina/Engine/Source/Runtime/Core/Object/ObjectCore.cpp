
#include "ObjectCore.h"
#include "Class.h"
#include "Object.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Engine/Engine.h"
#include "Core/Math/Math.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/ArrayProperty.h"
#include "Core/Reflection/Type/Properties/EnumProperty.h"
#include "Core/Reflection/Type/Properties/ObjectProperty.h"
#include "Core/Reflection/Type/Properties/StringProperty.h"
#include "Core/Reflection/Type/Properties/StructProperty.h"
#include "Package/Package.h"

namespace Lumina
{
    FMutex ObjectNameMutex;
    FMutex ObjectCreationMutex;
    
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

    
    FName MakeUniqueObjectName(const CClass* Class, const CPackage* Package, const FName& InBaseName)
    {
        FScopeLock Lock(ObjectNameMutex);
        
        FName BaseName = (InBaseName == NAME_None) ? Class->GetName() : InBaseName;

        FName FullName;
        if (Package)
        {
            TInlineString<256> Path;
            Path.append(Package->GetName().c_str())
            .append(".")
            .append(BaseName.c_str());

            FullName = Path.c_str();
        }
        
        // First, try the base name directly
        if (FindObjectFast(Class, FullName) == nullptr)
        {
            return BaseName;
        }

        // If it's taken, start appending an increasing index
        FName TestName;
        CObject* FoundObj = nullptr;
        
        do
        {
            int32_t Index = ++Class->ClassUnique;
            FString String = BaseName.ToString() + "_" + eastl::to_string(Index);
            TestName = FName(String);
            FoundObj = FindObjectFast(Class, TestName);
        }
        while (FoundObj != nullptr);

        return TestName;
    }


    CObject* StaticAllocateObject(FConstructCObjectParams& Params)
    {
        FScopeLock Lock(ObjectCreationMutex);
        
        CPackage* Package = nullptr;
        if (Params.Package != NAME_None)
        {
            Package = FindObject<CPackage>(Params.Package);
            if (Package == nullptr)
            {
                Package = NewObject<CPackage>(nullptr, Params.Package);
            }
        }
        
        FName UniqueName = MakeUniqueObjectName(Params.Class, Package, Params.Name);
        Params.Name = UniqueName;
        EObjectFlags Flags = Params.Flags;

        CObjectBase* Object = AllocateCObjectMemory(Params.Class, Flags);
        
        Memory::Memzero(Object, Params.Class->GetSize());
        new (Object) CObjectBase(const_cast<CClass*>(Params.Class), Params.Flags, Package, UniqueName);

        CObject* Obj = (CObject*)Object;
        Params.Class->ClassConstructor(FObjectInitializer(Obj, Package, Params));
        
        Obj->PostInitProperties();
        
        return Obj;
    }
    
    CObject* FindObjectFast(const CClass* InClass, const FName& QualifiedName)
    {
        FString PackageName = GetPackageFromQualifiedObjectName(QualifiedName.ToString());
        FString ObjectName = GetObjectNameFromQualifiedName(QualifiedName.ToString());
        
        CObject* ReturnObject = (CObject*)ObjectNameHashBucket.FindObject(PackageName.c_str(), ObjectName.c_str());
        return ReturnObject;
    }
    
    CObject* StaticLoadObject(const CClass* InClass, const FName& QualifiedName)
    {
        
        CObject* FoundObject = FindObjectFast(InClass, QualifiedName);

        if (FoundObject == nullptr || FoundObject->HasAnyFlag(OF_NeedsLoad))
        {
            FAssetManager* Manager = GEngine->GetEngineSubsystem<FAssetManager>();
            FAssetRequest* Request = Manager->LoadAsset(QualifiedName.ToString());
            Manager->FlushAsyncLoading();

            FoundObject = Request->GetPendingObject();
        }
        
        return FoundObject;
    }

    void ResolveObjectPath(FString& OutPath, const FStringView& InPath)
    {
        
    }

    FString GetPackageFromQualifiedObjectName(const FString& FullyQualifiedName)
    {
        size_t DotPos = FullyQualifiedName.rfind('.');
        if (DotPos != eastl::string::npos)
        {
            return FullyQualifiedName.substr(0, DotPos);
        }

        // If there is no package, we don't want to return the object name.
        return NAME_None.c_str();
    }

    LUMINA_API FString GetObjectNameFromQualifiedName(const FString& FullyQualifiedName)
    {
        size_t DotPos = FullyQualifiedName.rfind('.');
        if (DotPos != FString::npos && DotPos + 1 < FullyQualifiedName.size())
        {
            return FullyQualifiedName.substr(DotPos + 1);
        }

        return FullyQualifiedName;
    }

    FString RemoveNumberSuffixFromObject(const FString& ObjectName)
    {
        FString MutableString = ObjectName;
        
        // Check if the object part ends with _<number> and strip it
        size_t UnderscorePos = MutableString.find_last_of('_');
        if (UnderscorePos != FString::npos && UnderscorePos + 1 < MutableString.size())
        {
            bool bAllDigits = true;
            for (size_t i = UnderscorePos + 1; i < MutableString.size(); ++i)
            {
                if (!isdigit(MutableString[i]))
                {
                    bAllDigits = false;
                    break;
                }
            }

            if (bAllDigits)
            {
                MutableString = MutableString.substr(0, UnderscorePos);
            }
        }

        return MutableString;
    }

    FName MakeFullyQualifiedObjectName(const CPackage* Package, const FName& ObjectName)
    {
        TInlineString<256> Path;
        Path.append(Package->GetName().c_str())
        .append(".")
        .append(ObjectName.c_str());
        return Path.c_str();
    }


    void ResolveObjectName(FName& Name)
    {
        FString MutableName;
        
        // Check if the base name ends with _<number> and remove it
        size_t Pos = MutableName.find_last_of('_');
        if (Pos != FString::npos && Pos + 1 < MutableName.size())
        {
            // Check if the part after '_' is numeric
            bool bAllDigits = true;
            for (size_t i = Pos + 1; i < MutableName.size(); ++i)
            {
                if (!isdigit(MutableName[i]))
                {
                    bAllDigits = false;
                    break;
                }
            }

            if (bAllDigits)
            {
                MutableName = MutableName.substr(0, Pos);
            }
        }

        Name = FName(MutableName);
    }

    bool IsValid(CObjectBase* Obj)
    {
        if (Obj == nullptr)
        {
            return false;
        }

        if (Obj->HasAnyFlag(OF_NeedsLoad | OF_MarkedGarbage))
        {
            return false;
        }

        return true;
    }

    CObject* NewObject(CClass* InClass, const CPackage* Package, const FName& Name, EObjectFlags Flags)
    {
        FConstructCObjectParams Params(InClass);
        Params.Name = Name;
        Params.Flags = Flags;
        
        if (Package)
        {
            Params.Package = Package->GetName();
        }

        return StaticAllocateObject(Params);
    }

    void GetObjectsWithPackage(CPackage* Package, TVector<CObject*>& OutObjects)
    {
        Assert(Package != nullptr);
        
        for (SIZE_T i = 0; i < GObjectVector.size(); ++i)
        {
            CObjectBase* Object = GObjectVector[i];
            if (Object->GetPackage() == Package)
            {
                OutObjects.push_back((CObject*)Object);
            }
        }
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

        // Link this class to its parent. (if it has one).
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
        FinalClass = ::new (FinalClass) CStruct(nullptr, FName(Params.Name), Params.SizeOf, Params.AlignOf, EObjectFlags::OF_None);
        *OutStruct = FinalClass;

        CObjectForceRegistration(FinalClass);
        
        InitializeAndCreateFProperties(FinalClass, Params.Params, Params.NumProperties);

        // Link this class to its parent. (if it has one).
        FinalClass->Link();
    }
}
