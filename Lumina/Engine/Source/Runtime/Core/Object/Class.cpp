#include "Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    
    LUMINA_API void AllocateStaticClass(const TCHAR* Package, const TCHAR* Name, CClass** OutClass, uint32 Size, uint32 Alignment, CClass* (*SuperClassFn)(), CClass::ClassConstructorType InClassConstructor)
    {
        Assert(*OutClass == nullptr);
        
        *OutClass = (CClass*)FMemory::Malloc(sizeof(CClass), alignof(CClass));
        *OutClass = ::new (*OutClass) CClass(Package, FName(Name), Size, Alignment, EObjectFlags::OF_None, InClassConstructor);
        
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

    CObject* CClass::CreateDefaultObject()
    {
        if (ClassDefaultObject != nullptr)
        {
            return ClassDefaultObject;
        }

        FString DefaultObjectName = GetName().c_str();
        DefaultObjectName += "_CDO";
        
        FConstructCObjectParams Params(this);
        Params.Flags |= EObjectFlags::OF_DefaultObject;
        Params.Name = FName(DefaultObjectName);
        
        ClassDefaultObject = StaticAllocateObject(Params);
        
        return ClassDefaultObject;
        
    }

}
