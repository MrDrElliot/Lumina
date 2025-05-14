#pragma once

#include <utility>
#include "ObjectFlags.h"
#include "Module/API.h"
#include "Initializer/ObjectInitializer.h"
#include "Memory/RefCounted.h"

enum ENoInit : int;

namespace Lumina
{
    class CObjectBase;
    class CClass;

    extern LUMINA_API TVector<CObjectBase*> PendingDeletes;
    extern LUMINA_API TFixedVector<CObjectBase*, 2024> GObjectVector;
    
    /** Low level implementation of a CObject */
    class CObjectBase : public IRefCountedObject
    {
    public:

        LUMINA_API CObjectBase();
        LUMINA_API CObjectBase(ENoInit);
        LUMINA_API virtual ~CObjectBase() override = default;
        
        LUMINA_API CObjectBase(EObjectFlags InFlags);
        LUMINA_API CObjectBase(CClass* InClass, EObjectFlags InFlags, const TCHAR* Package, FName InName);

        // Begin IRefCountedObject
        LUMINA_API uint32 AddRef() const override;
        LUMINA_API uint32 Release() const override;
        LUMINA_API uint32 GetRefCount() const override { return RefCount; }
        //~ End IRefCountedObject
        
        
        LUMINA_API void BeginRegister();
        LUMINA_API void FinishRegister(CClass* InClass, const TCHAR* InName);

        LUMINA_API EObjectFlags GetFlags() const { return ObjectFlags; }
        LUMINA_API void SetFlag(EObjectFlags Flags) { ObjectFlags = Flags; }
        LUMINA_API bool HasAnyFlag(EObjectFlags Flag) const { return EnumHasAnyFlags(ObjectFlags, Flag); }
        LUMINA_API bool HasAllFlags(EObjectFlags Flags) const { return EnumHasAllFlags(ObjectFlags, Flags); }

        LUMINA_API bool IsPendingDelete() const { return HasAnyFlag(OF_PendingDelete); }

    private:

        LUMINA_API void AddObject(FName Name, int32 InInternalIndex = -1);
        
    public:
        
        /** Force any base classes to be registered first. */
        LUMINA_API virtual void RegisterDependencies() { }

        /** Returns the CClass that defines the fields of this object */
        FORCEINLINE CClass* GetClass() const
        {
            return ClassPrivate;
        }

        FORCEINLINE FName GetName() const
        {
            return NamePrivate;
        }

        FORCEINLINE const TCHAR* GetPackage() const
        {
            return PackagePrivate;    
        }

        LUMINA_API void GetPath(FString& OutPath);
        LUMINA_API FString GetPathName() const;
    
    private:
        
        template<typename TClassType>
        static FORCEINLINE bool IsAHelper(const TClassType* Class, const TClassType* TestClass)
        {
            return Class->IsChildOf(TestClass);
        }
        
    public:

        // This exists to fix the cyclical dependency between CObjectBase and CClass.
        template<typename OtherClassType>
        FORCEINLINE bool IsA(OtherClassType Base) const
        {
            const CClass* SomeBase = Base;
            (void)SomeBase;

            const CClass* ThisClass = GetClass();

            // Stop compiler from doing un-necessary branching for nullptr checks.
            ASSUME(SomeBase);
            ASSUME(ThisClass);

            return IsAHelper(ThisClass, SomeBase);
            
        }
        
        template<typename T>
        bool IsA() const
        {
            return IsA(T::StaticClass());
        }

    private:

        /** Flags used to track various object states. */
        mutable EObjectFlags    ObjectFlags;

        /** Class this object belongs to */
        CClass*                 ClassPrivate = nullptr;

        /** Logical name of this object. */
        FName                   NamePrivate;

        /** Package to represent on disk */
        const TCHAR*            PackagePrivate;
        
        /** Internal index into the global object array. */
        int32                   InternalIndex;

        /** Internal reference count to this object */
        mutable uint32          RefCount = 0;
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
        const TCHAR* Package;
        const TCHAR* Name;
    };

    struct FEnumRegisterCompiledInInfo
    {
        class CEnum* (*RegisterFn)();
        const TCHAR* Name;
        
    };
    

    /** Involked from static constructor in generated code */
    LUMINA_API void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Package, const TCHAR* Name);

    LUMINA_API void RegisterCompiledInInfo(CEnum* (*RegisterFn)(), const FEnumRegisterCompiledInInfo& Info);

    LUMINA_API CEnum* GetStaticEnum(CEnum* (*RegisterFn)(), const TCHAR* Name);
    
    LUMINA_API void RegisterCompiledInInfo(const FClassRegisterCompiledInInfo* Info, SIZE_T NumClassInfo);

    LUMINA_API void RegisterCompiledInInfo(const FEnumRegisterCompiledInInfo* EnumInfo, SIZE_T NumEnumInfo, const FClassRegisterCompiledInInfo* ClassInfo, SIZE_T NumClassInfo);

    
    LUMINA_API void CObjectForceRegistration(CObjectBase* Object);

    
    void ProcessNewlyLoadedCObjects();
}
