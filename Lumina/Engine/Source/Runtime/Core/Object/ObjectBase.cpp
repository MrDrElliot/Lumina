
#include "ObjectBase.h"
#include "DeferredRegistry.h"
#include "Lumina.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{

    

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


    CObjectBase::CObjectBase(EObjectFlags InFlags)
        : ObjectFlags(InFlags)
        , ClassPrivate(nullptr)
        , NamePrivate("")
    {
    }

    CObjectBase::CObjectBase(CClass* InClass, EObjectFlags InFlags, FName InName)
        : ObjectFlags(InFlags)
        , ClassPrivate(InClass)
        , NamePrivate(InName)
    {
        
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
            Assert(!GFirstPendingRegistrant);
            GFirstPendingRegistrant = PendingRegistrant;
        }

        GLastPendingRegistrant = PendingRegistrant;
    }

    void CObjectBase::FinishRegister()
    {
        LOG_WARN("Registered Object: {0}", NamePrivate.c_str());
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
            Object->FinishRegister();
            
        }
    }

    void ProcessNewlyLoadedCObjects()
    {
        FClassDeferredRegistry& ClassRegistry = FClassDeferredRegistry::Get();
        
        while (GFirstPendingRegistrant || ClassRegistry.HasPendingRegistrations())
        {
            ProcessRegistrants();
            ClassRegistry.ProcessRegistrations();
        }
    }
    

    void RegisterCompiledInInfo(CClass* (*RegisterFn)(), const TCHAR* Name)
    {
        FClassDeferredRegistry::Get().AddRegistration(RegisterFn);
    }

    void RegisterCompiledInInfo(const FClassRegisterCompiledInInfo* Info, SIZE_T NumClassInfo)
    {
        for (const FClassRegisterCompiledInInfo* It = Info; It != Info + NumClassInfo; ++It)
        {
            RegisterCompiledInInfo(It->RegisterFn, Info->Name);
        }
    }
    
}
