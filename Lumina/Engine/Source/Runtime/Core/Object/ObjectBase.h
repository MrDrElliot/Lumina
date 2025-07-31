#pragma once

#include "Initializer/ObjectInitializer.h"
#include "Memory/RefCounted.h"
#include "Module/API.h"
#include "ObjectFlags.h"
#include <utility>

namespace Lumina
{
    class CPackage;
    class CObjectBase;
    class CClass;
    
    /** Low level implementation of a CObject */
    class CObjectBase : public IRefCountedObject
    {
    public:
        friend class FCObjectArray;
        friend class CPackage;
        
        LUMINA_API CObjectBase();
        LUMINA_API virtual ~CObjectBase() override;
        
        LUMINA_API CObjectBase(EObjectFlags InFlags);
        LUMINA_API CObjectBase(CClass* InClass, EObjectFlags InFlags, CPackage* Package, FName InName);

        // Begin IRefCountedObject
        LUMINA_API uint32 AddRef() const override;
        LUMINA_API uint32 Release() override;
        LUMINA_API uint32 GetRefCount() const override { return RefCount; }
        //~ End IRefCountedObject
        
        
        LUMINA_API void BeginRegister();
        LUMINA_API void FinishRegister(CClass* InClass, const TCHAR* InName);

        LUMINA_API EObjectFlags GetFlags() const { return ObjectFlags; }

        LUMINA_API void ClearFlags(EObjectFlags Flags) { EnumRemoveFlags(ObjectFlags, Flags); }
        LUMINA_API void SetFlag(EObjectFlags Flags) { EnumAddFlags(ObjectFlags, Flags); }
        LUMINA_API bool HasAnyFlag(EObjectFlags Flag) const { return EnumHasAnyFlags(ObjectFlags, Flag); }
        LUMINA_API bool HasAllFlags(EObjectFlags Flags) const { return EnumHasAllFlags(ObjectFlags, Flags); }
        

        LUMINA_API void Rename(TCHAR* NewName);
        
        /** Adds this object to the pending deletion queue, recommended to manually null the memory to avoid stale access */
        LUMINA_API void MarkGarbage();

        /** Forcefully destroys this object now, without adding to the queue. Recommended to manually null the memory to avoid stale access */
        LUMINA_API void DestroyNow();

        /** Has this object previously been marked for destruction? */
        LUMINA_API bool IsMarkedGarbage() const { return HasAnyFlag(OF_MarkedGarbage); }

        /** Called just before the destructor is called and the memory is freed */
        LUMINA_API virtual void OnDestroy() { }

        /** Called when the object is initially marked as garbage */
        LUMINA_API virtual void OnMarkedGarbage() { }
        
    private:

        LUMINA_API void AddObject(const FName& Name, uint32 InInternalIndex = -1);
        
    public:
        
        /** Force any base classes to be registered first. */
        LUMINA_API virtual void RegisterDependencies() { }

        /** Returns the CClass that defines the fields of this object */
        CClass* GetClass() const
        {
            return ClassPrivate;
        }

        /** Get the internal low level name of this object */
        FName GetName() const
        {
            return NamePrivate;
        }

        /** Get the internal package this object came from (script, plugin, package, etc). */
        CPackage* GetPackage() const
        {
            return PackagePrivate;
        }

        /** Loader index from asset loading */
        LUMINA_API int64 GetLoaderIndex() const
        {
            return LoaderIndex;
        }

        LUMINA_API void GetPath(FString& OutPath) const;
        LUMINA_API FString GetPathName() const;
        LUMINA_API FName GetFullyQualifiedName() const;
    
    private:
        
        template<typename TClassType>
        static bool IsAHelper(const TClassType* Class, const TClassType* TestClass)
        {
            return Class->IsChildOf(TestClass);
        }
        
    public:

        // This exists to fix the cyclical dependency between CObjectBase and CClass.
        template<typename OtherClassType>
        bool IsA(OtherClassType Base) const
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
        CPackage*               PackagePrivate = nullptr;
        
        /** Internal index into the global object array. */
        uint32                  InternalIndex;

        /** Index into this object's package export map */
        int64                   LoaderIndex = 0;

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

    struct FStructRegisterCompiledInInfo
    {
        class CStruct* (*RegisterFn)();
        const TCHAR* Name;
    };

    struct FEnumRegisterCompiledInInfo
    {
        class CEnum* (*RegisterFn)();
        const TCHAR* Name;
        
    };


    LUMINA_API void ShutdownCObjectSystem();

    /** Involked from static constructor in generated code */
    LUMINA_API void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Package, const TCHAR* Name);

    LUMINA_API void RegisterCompiledInInfo(CEnum* (*RegisterFn)(), const FEnumRegisterCompiledInInfo& Info);

    LUMINA_API void RegisterCompiledInInfo(CStruct* (*RegisterFn)(), const FStructRegisterCompiledInInfo& Info);

    LUMINA_API CEnum* GetStaticEnum(CEnum* (*RegisterFn)(), const TCHAR* Name);
    
    LUMINA_API void RegisterCompiledInInfo(const FClassRegisterCompiledInInfo* Info, SIZE_T NumClassInfo);

    LUMINA_API void RegisterCompiledInInfo(const FEnumRegisterCompiledInInfo* EnumInfo, SIZE_T NumEnumInfo, const FClassRegisterCompiledInInfo* ClassInfo, SIZE_T NumClassInfo);

    LUMINA_API void RegisterCompiledInInfo(const FEnumRegisterCompiledInInfo* EnumInfo, SIZE_T NumEnumInfo, const FClassRegisterCompiledInInfo* ClassInfo, SIZE_T NumClassInfo, const FStructRegisterCompiledInInfo* StructInfo, SIZE_T NumStructInfo);

    
    LUMINA_API void CObjectForceRegistration(CObjectBase* Object);

    
    void ProcessNewlyLoadedCObjects();
}
