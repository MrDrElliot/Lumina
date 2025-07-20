
#include "ObjectBase.h"

#include "Class.h"
#include "DeferredRegistry.h"
#include "Lumina.h"
#include "GarbageCollection/GarbageCollector.h"
#include "Log/Log.h"
#include "Memory/Memory.h"
#include "Package/Package.h"

namespace Lumina
{

    LUMINA_API FObjectNameHashBucket               ObjectNameHashBucket;    
    LUMINA_API TFixedVector<CObjectBase*, 2024>    GObjectVector;

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
        , LoaderIndex(0)
    {
        FObjectInitializer* Initializer = FObjectInitializer::Get();
        NamePrivate = Initializer->Params.Name;
        ClassPrivate = const_cast<CClass*>(Initializer->Params.Class);
        PackagePrivate = Initializer->Package;

        AddObject(NamePrivate, GObjectVector.size());

    }

    CObjectBase::~CObjectBase()
    {
        Assert(IsMarkedGarbage())

        FName PackageName = PackagePrivate ? PackagePrivate->GetName() : NAME_None;

        ObjectNameHashBucket.RemoveObject(PackageName, NamePrivate);
        
        if (InternalIndex < GObjectVector.size() - 1)
        {
            eastl::swap(GObjectVector[InternalIndex], GObjectVector.back());

            // Fix the InternalIndex of the moved object
            GObjectVector[InternalIndex]->InternalIndex = InternalIndex;
        }

        GObjectVector.pop_back();

    }

    CObjectBase::CObjectBase(EObjectFlags InFlags)
        : ObjectFlags(InFlags)
        , NamePrivate(NAME_None)
        , InternalIndex(0)
        , LoaderIndex(0)
    {
    }

    CObjectBase::CObjectBase(CClass* InClass, EObjectFlags InFlags, CPackage* Package, FName InName)
        : ObjectFlags(InFlags)
        , ClassPrivate(InClass)
        , NamePrivate(Memory::Move(InName))
        , PackagePrivate(Package)
        , InternalIndex(0)
        , LoaderIndex(0)
    {
    }

    uint32 CObjectBase::AddRef() const
    {
        return ++RefCount;
    }

    uint32 CObjectBase::Release()
    {
        if (--RefCount == 0)
        {
            MarkGarbage();
        }

        return RefCount;
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

    void CObjectBase::Rename(TCHAR* NewName)
    {
        
    }

    void CObjectBase::MarkGarbage()
    {
        GarbageCollection::AddGarbage(this);
    }

    void CObjectBase::DestroyNow()
    {
        Assert(!IsMarkedGarbage())
        SetFlag(OF_MarkedGarbage);
        OnMarkedGarbage();
        OnDestroy();

        Memory::Delete(this);
    }

    void CObjectBase::AddObject(const FName& Name, SIZE_T InInternalIndex)
    {
        InternalIndex = InInternalIndex;

        FName PackageName = PackagePrivate ? PackagePrivate->GetName() : NAME_None;
        ObjectNameHashBucket.AddObject(PackageName, Name, this);
        
        GObjectVector.push_back(this);
    }

    void CObjectBase::GetPath(FString& OutPath) const
    {
        OutPath = GetPathName();
    }

    FString CObjectBase::GetPathName() const
    {
        if (PackagePrivate == nullptr)
        {
            return ".";
        }
        return GetPackage()->GetName().ToString();
    }

    FName CObjectBase::GetFullyQualifiedName() const
    {
        TInlineString<256> Path;
        Path.append(GetPathName().c_str())
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
            Memory::Delete(PendingRegistrant);
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
    
    static void LoadAllCompiledInEnumsAndStructs()
    {
        FEnumDeferredRegistry& EnumRegistry = FEnumDeferredRegistry::Get();
        FStructDeferredRegistry& StructRegistry = FStructDeferredRegistry::Get();

        EnumRegistry.ProcessRegistrations();
        StructRegistry.ProcessRegistrations();
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
            LoadAllCompiledInEnumsAndStructs();

            if (ClassRegistry.HasPendingRegistrations())
            {
                ClassRegistry.ProcessRegistrations([] (CClass& Class)
                {
                    Class.GetDefaultObject();
                });
            }
        }
        
    }

    void ShutdownCObjectSystem()
    {
        while (!GObjectVector.empty())
        {
            CObjectBase* Base = GObjectVector.back();
            Base->MarkGarbage();
            GarbageCollection::CollectGarbage();
        }

        ObjectNameHashBucket.Clear();
        
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
