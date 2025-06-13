
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
        , InternalIndex(INDEX_NONE)
    {
        FObjectInitializer* Initializer = FObjectInitializer::Get();
        NamePrivate = Initializer->Params.Name;
        ClassPrivate = const_cast<CClass*>(Initializer->Params.Class);
        PackagePrivate = Initializer->Params.Package;

        AddObject(NamePrivate, (int32)GObjectVector.size());

    }

    CObjectBase::~CObjectBase()
    {
        LOG_INFO("Deleting Object: {}", GetName());

        ObjectNameHash.erase(NamePrivate);
        if (InternalIndex < (int32)GObjectVector.size() - 1)
        {
            eastl::swap(GObjectVector[InternalIndex], GObjectVector.back());

            // Fix the InternalIndex of the moved object
            GObjectVector[InternalIndex]->InternalIndex = InternalIndex;
        }

        GObjectVector.pop_back();

    }

    CObjectBase::CObjectBase(EObjectFlags InFlags)
        : ObjectFlags(InFlags)
        , ClassPrivate(nullptr)
        , PackagePrivate(nullptr)
        , NamePrivate(NAME_None)
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
    }

    uint32 CObjectBase::AddRef() const
    {
        return ++RefCount;
    }

    uint32 CObjectBase::Release() const
    {
        uint32 NewRefCount = --RefCount;

        // We use one, because the last element is in the object array.
        if (NewRefCount == 1)
        {
            CObjectBase* MutableThis =const_cast<CObjectBase*>(this);
            FMemory::Delete(MutableThis);
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

        AddObject(NamePrivate, (int32)GObjectVector.size());
    }

    void CObjectBase::AddObject(FName Name, int32 InInternalIndex)
    {
        LOG_INFO("Adding CObject - {}", Name);
        InternalIndex = InInternalIndex;
        ObjectNameHash.insert_or_assign(Name, this);
        GObjectVector.push_back(this);
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

    static void LoadAllCompiledInDefaultProperties(CStruct* NewClass)
    {
        
    }

    static void LoadAllCompiledInDefaultProperties(CEnum* NewClass)
    {
        
    }
    
    void ProcessNewlyLoadedCObjects()
    {
        FClassDeferredRegistry& ClassRegistry = FClassDeferredRegistry::Get();
        FEnumDeferredRegistry& EnumRegistry = FEnumDeferredRegistry::Get();
        FStructDeferredRegistry& StructRegistry = FStructDeferredRegistry::Get();

        while (GFirstPendingRegistrant 
            || ClassRegistry.HasPendingRegistrations()
            || EnumRegistry.HasPendingRegistrations()
            || StructRegistry.HasPendingRegistrations())
        {
            ProcessRegistrants();
            ClassRegistry.ProcessRegistrations(LoadAllCompiledInDefaultProperties);
            StructRegistry.ProcessRegistrations(LoadAllCompiledInDefaultProperties);
            EnumRegistry.ProcessRegistrations(LoadAllCompiledInDefaultProperties);
        }
        
    }

    void ShutdownCObjectSystem()
    {
        while (!GObjectVector.empty())
        {
            CObjectBase* Base = GObjectVector.back();
            FMemory::Delete(Base);
        }

        ObjectNameHash.clear();

    }


    void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Package, const TCHAR* Name)
    {
        FClassDeferredRegistry::Get().AddRegistration(RegisterFn);
    }

    void RegisterCompiledInInfo(CEnum*(* RegisterFn)(), const FEnumRegisterCompiledInInfo& Info)
    {
        FEnumDeferredRegistry::Get().AddRegistration(RegisterFn);
    }

    void RegisterCompiledInInfo(CStruct*(* RegisterFn)(), const FStructRegisterCompiledInInfo& Info)
    {
        FStructDeferredRegistry::Get().AddRegistration(RegisterFn);
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

    void RegisterCompiledInInfo(const FEnumRegisterCompiledInInfo* EnumInfo, SIZE_T NumEnumInfo, const FClassRegisterCompiledInInfo* ClassInfo, SIZE_T NumClassInfo, const FStructRegisterCompiledInInfo* StructInfo, SIZE_T NumStructInfo)
    {
        for (const FClassRegisterCompiledInInfo* It = ClassInfo; It != ClassInfo + NumClassInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, ClassInfo->Package, ClassInfo->Name);
        }

        for (const FEnumRegisterCompiledInInfo* It = EnumInfo; It != EnumInfo + NumEnumInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, *It);
        }

        for (const FStructRegisterCompiledInInfo* It = StructInfo; It != StructInfo + NumStructInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, *It);
        }
    }
}
