
#include "ObjectBase.h"

#include "Class.h"
#include "DeferredRegistry.h"
#include "Lumina.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{

    TVector<CObjectBase*> PendingDeletes;
    THashMap<FName, CObjectBase*> ObjectNameHash;
    TFixedVector<CObjectBase*, 2024> GObjectVector;

    struct FPendingRegistrantInfo
    {
        static TVector<CObjectBase*>& Get()
        {
            static TVector<CObjectBase*> PendingRegistrantInfo;
            return PendingRegistrantInfo;
        }
    };
    
    struct FPendingRegistrant
    {
        CObjectBase*        Object;
        FPendingRegistrant* Next;
    };

    static FPendingRegistrant* GFirstPendingRegistrant = nullptr;
    static FPendingRegistrant* GLastPendingRegistrant = nullptr;


    //-----------------------------------------------------------------------------------------------


    /** Reinitialize properties because they were already set, and we don't want to clear them. */
    CObjectBase::CObjectBase()
        : ObjectFlags()
        , InternalIndex(0)
    {
        FObjectInitializer* Initializer = FObjectInitializer::Get();
        NamePrivate = Initializer->Params.Name;
        ClassPrivate = const_cast<CClass*>(Initializer->Params.Class);
        PackagePrivate = Initializer->Params.Package;

        ObjectNameHash.insert_or_assign(NamePrivate, this);
        GObjectVector.push_back(this);
    }

    CObjectBase::CObjectBase(ENoInit)
        : ObjectFlags()
        , PackagePrivate(nullptr)
        , InternalIndex(0)
    {
        //.. Internal use zero-init constructor.
    }

    CObjectBase::CObjectBase(EObjectFlags InFlags)
        : ObjectFlags(InFlags)
        , ClassPrivate(nullptr)
        , PackagePrivate(nullptr)
        , NamePrivate("")
        , InternalIndex(0)
    {
    }

    CObjectBase::CObjectBase(CClass* InClass, EObjectFlags InFlags, const TCHAR* Package, FName InName)
        : ObjectFlags(InFlags)
        , ClassPrivate(InClass)
        , PackagePrivate(Package)
        , NamePrivate(InName)
        , InternalIndex(0)
    {
        ObjectNameHash.insert_or_assign(NamePrivate, this);
        GObjectVector.push_back(this);
    }

    uint32 CObjectBase::AddRef() const
    {
        return ++RefCount;
    }

    uint32 CObjectBase::Release() const
    {
        uint32 NewRefCount = --RefCount;
    
        if (NewRefCount == 0)
        {
            ObjectFlags |= OF_PendingDelete;
            PendingDeletes.push_back(const_cast<CObjectBase*>(this));
        }

        return NewRefCount;
    }

    void CObjectBase::BeginRegister()
    {
        FPendingRegistrant* PendingRegistrant = new FPendingRegistrant(this);
        FPendingRegistrantInfo::Get().push_back(this);

        if (GLastPendingRegistrant)
        {
            GLastPendingRegistrant->Next = PendingRegistrant;
        }
        else
        {
            Assert(!GFirstPendingRegistrant)
            GFirstPendingRegistrant = PendingRegistrant;
        }

        GLastPendingRegistrant = PendingRegistrant;
    }

    void CObjectBase::FinishRegister(CClass* InClass, const TCHAR* InName)
    {
        Assert(ClassPrivate == nullptr)
        ClassPrivate = InClass;
    }

    void CObjectBase::AddObject(FName Name, int32 InInternalIndex)
    {
        
    }

    void CObjectBase::GetPath(FString& OutPath)
    {
        OutPath = GetPathName();
    }

    FString CObjectBase::GetPathName() const
    {
        TInlineString<256> Path;
        Path.append(StringUtils::FromWideString(GetPackage()).c_str())
        .append("/")
        .append(GetClass()->GetName().c_str())
        .append(".")
        .append(GetName().c_str());

        
        return Path.c_str();
    }

    //-----------------------------------------------------------------------------------------------
    

    static void DequeuePendingAutoRegistrations(TVector<FPendingRegistrant>& OutPending)
    {
        FPendingRegistrant* NextPendingRegistrant = GFirstPendingRegistrant;
        GFirstPendingRegistrant = nullptr;
        GLastPendingRegistrant = nullptr;
        while(NextPendingRegistrant)
        {
            FPendingRegistrant* PendingRegistrant = NextPendingRegistrant;
            OutPending.push_back(*PendingRegistrant);
            NextPendingRegistrant = PendingRegistrant->Next;
            FMemory::Delete(PendingRegistrant);
        }
    }

    static void ProcessRegistrants()
    {
        TVector<FPendingRegistrant> PendingRegistrants;
        DequeuePendingAutoRegistrations(PendingRegistrants);

        for (SIZE_T Index = 0; Index < PendingRegistrants.size(); ++Index)
        {
            const FPendingRegistrant& PendingRegistrant = PendingRegistrants[Index];

            CObjectForceRegistration(PendingRegistrant.Object);

            // Register any new results.
            DequeuePendingAutoRegistrations(PendingRegistrants);
        }
    }
    
    void CObjectForceRegistration(CObjectBase* Object)
    {
        TVector<CObjectBase*>& Pending = FPendingRegistrantInfo::Get();
        int32 Index = VectorFindIndex(Pending, Object);
        
        if (Index != INDEX_NONE)
        {
            // Remove here so it doesn't happen twice.
            Pending.erase(Pending.begin() + Index);
            Object->FinishRegister(CClass::StaticClass(), TEXT(""));
            
        }
    }

    static void LoadAllCompiledInDefaultProperties(CClass* NewClass)
    {
        NewClass->GetDefaultObject();
    }

    void ProcessNewlyLoadedCObjects()
    {
        FClassDeferredRegistry& ClassRegistry = FClassDeferredRegistry::Get();
        FEnumDeferredRegistry& EnumRegistry = FEnumDeferredRegistry::Get();

        while (GFirstPendingRegistrant 
            || ClassRegistry.HasPendingRegistrations()
            || EnumRegistry.HasPendingRegistrations())
        {
            ProcessRegistrants();
            ClassRegistry.ProcessRegistrations(LoadAllCompiledInDefaultProperties);
            EnumRegistry.ProcessRegistrations();
        }
        
    }
    

    void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Package, const TCHAR* Name)
    {
        FClassDeferredRegistry::Get().AddRegistration(RegisterFn);
    }

    void RegisterCompiledInInfo(CEnum*(* RegisterFn)(), const FEnumRegisterCompiledInInfo& Info)
    {
        FEnumDeferredRegistry::Get().AddRegistration(RegisterFn);
    }

    CEnum* GetStaticEnum(CEnum*(* RegisterFn)(), const TCHAR* Name)
    {
        return RegisterFn();
    }

    void RegisterCompiledInInfo(const FClassRegisterCompiledInInfo* Info, SIZE_T NumClassInfo)
    {
        for (const FClassRegisterCompiledInInfo* It = Info; It != Info + NumClassInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, Info->Package, Info->Name);
        }
    }

    void RegisterCompiledInInfo(const FEnumRegisterCompiledInInfo* EnumInfo, SIZE_T NumEnumInfo, const FClassRegisterCompiledInInfo* ClassInfo, SIZE_T NumClassInfo)
    {
        for (const FClassRegisterCompiledInInfo* It = ClassInfo; It != ClassInfo + NumClassInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, ClassInfo->Package, ClassInfo->Name);
        }

        for (const FEnumRegisterCompiledInInfo* It = EnumInfo; It != EnumInfo + NumEnumInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, *It);
        }
    }
}
