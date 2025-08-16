#include "Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "glm/glm.hpp"
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
    

    CObject* CClass::CreateDefaultObject()
    {
        Assert(ClassDefaultObject == nullptr)


        Link();
        
        
        FString DefaultObjectName = GetName().c_str();
        DefaultObjectName += "_CDO";
        
        FConstructCObjectParams Params(this);
        Params.Flags |= OF_DefaultObject;
        Params.Name = FName(DefaultObjectName);
        
        if (GetPackage())
        {
            Params.Package = GetPackage()->GetName();
        }
        
        ClassDefaultObject = StaticAllocateObject(Params);

        ClassDefaultObject->PostCreateCDO();
        
        return ClassDefaultObject;
        
    }


    static CStruct* StaticGetBaseStructureInternal(FName Name)
    {
        FName QualifiedName = FName(FString("script://lumina.") + Name.c_str());
        CStruct* Result = (CStruct*)FindObjectFast(CStruct::StaticClass(), QualifiedName);
        
        return Result;
    }

    CStruct* TBaseStructure<glm::vec<2, float>>::Get()
    {
	    static CStruct* Struct = StaticGetBaseStructureInternal("vec2");
        return Struct;
    }

    CStruct* TBaseStructure<glm::vec<3, float>>::Get()
    {
        static CStruct* Struct = StaticGetBaseStructureInternal("vec3");
        return Struct;
    }

    CStruct* TBaseStructure<glm::vec<4, float>>::Get()
    {
        static CStruct* Struct = StaticGetBaseStructureInternal("vec4");
        return Struct;   
    }

    CStruct* TBaseStructure<FTransform>::Get()
    {
        static CStruct* Struct = StaticGetBaseStructureInternal("FTransform");
        return Struct;   
    }
}
