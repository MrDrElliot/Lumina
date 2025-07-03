#include "Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Package/Package.h"

namespace Lumina
{
    
    LUMINA_API void AllocateStaticClass(const TCHAR* Package, const TCHAR* Name, CClass** OutClass, uint32 Size, uint32 Alignment, CClass* (*SuperClassFn)(), CClass::ClassConstructorType InClassConstructor)
    {
        Assert(*OutClass == nullptr)
        
        CPackage* PackageObject = nullptr;

        if (Package && Package[0] != '\0')
        {
            PackageObject = FindObject<CPackage>(Package);
            if (PackageObject == nullptr)
            {
                PackageObject = NewObject<CPackage>(nullptr, Package);
            }
        }
        
        *OutClass = (CClass*)Memory::Malloc(sizeof(CClass), alignof(CClass));
        *OutClass = ::new (*OutClass) CClass(PackageObject, FName(Name), Size, Alignment, EObjectFlags::OF_None, InClassConstructor);
        
        CClass* NewClass = *OutClass;
        CClass* SuperClass = SuperClassFn();
        
        bool bValidSuperClass = (SuperClass != NewClass);
        NewClass->SetSuperStruct(bValidSuperClass ? SuperClass : nullptr);

        NewClass->RegisterDependencies();
        NewClass->BeginRegister();
    }


    //-----------------------------------------------------------------------------------------------------------------------------------------

    
    IMPLEMENT_INTRINSIC_CLASS(CClass, CStruct, LUMINA_API)


    //-----------------------------------------------------------------------------------------------

    void CClass::SerializeClassProperties(FArchive& Ar, void* Data)
    {
        FProperty* Current = LinkedProperty;
        while (Current != nullptr)
        {
            void* ValuePtr = Current->GetValuePtr<void>(Data);
            Current->Serialize(Ar, ValuePtr);
            
            Current = (FProperty*)Current->Next;
        }
    }

    CObject* CClass::CreateDefaultObject()
    {
        Assert(ClassDefaultObject == nullptr)
        FString DefaultObjectName = GetName().c_str();
        DefaultObjectName += "_CDO";
        
        FConstructCObjectParams Params(this);
        Params.Flags |= EObjectFlags::OF_DefaultObject;
        Params.Name = FName(DefaultObjectName);
        
        if (GetPackage())
        {
            Params.Package = GetPackage()->GetName();
        }
        
        ClassDefaultObject = StaticAllocateObject(Params);

        ClassDefaultObject->PostCreateCDO();
        
        return ClassDefaultObject;
        
    }

}
