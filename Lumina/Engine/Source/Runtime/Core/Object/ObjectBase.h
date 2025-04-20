#pragma once

#include <utility>
#include "ObjectFlags.h"
#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CClass;

    /** Low level implementation of a CObject */
    class CObjectBase
    {
    public:

        LUMINA_API CObjectBase(EObjectFlags InFlags);
        LUMINA_API CObjectBase(CClass* InClass, EObjectFlags InFlags, FName InName);
        

        LUMINA_API void BeginRegister();
        LUMINA_API void FinishRegister();

        LUMINA_API EObjectFlags GetFlags() const { return ObjectFlags; }
        LUMINA_API bool HasAnyFlag(EObjectFlags Flag) { return EnumHasAnyFlags(ObjectFlags, Flag); }
        LUMINA_API bool HasAllFlags(EObjectFlags Flags) { return EnumHasAllFlags(ObjectFlags, Flags); }


        /** Returns the CClass that defines the fields of this object */
        FORCEINLINE CClass* GetClass() const
        {
            return ClassPrivate;
        }

        FORCEINLINE FName GetName() const
        {
            return NamePrivate;
        }

    private:

        /** Flags used to track various object states */
        EObjectFlags    ObjectFlags;

        /** Class this object belongs to */
        CClass*         ClassPrivate = nullptr;

        /** Logical name of this object. */
        FName           NamePrivate;
    };

//---------------------------------------------------------------------------------------------------
    

    /** Helper for static registration, mostly from LRT code */
    struct FRegisterCompiledInInfo
    {
        template<typename ... Args>
        FRegisterCompiledInInfo(Args&& ... args)
        {
            RegisterCompiledInInfo(std::forward<Args>(args)...);
        }
    };

    struct FClassRegisterCompiledInInfo
    {
        class CClass* (*RegisterFn)();
        const TCHAR* Name;
    };
    

    /** Involked from static constructor in generated code */
    LUMINA_API void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Name);

    
    
    LUMINA_API void RegisterCompiledInInfo(const FClassRegisterCompiledInInfo* Info, SIZE_T NumClassInfo);


    
    LUMINA_API void CObjectForceRegistration(CObjectBase* Object);

    
    void ProcessNewlyLoadedCObjects();
}
