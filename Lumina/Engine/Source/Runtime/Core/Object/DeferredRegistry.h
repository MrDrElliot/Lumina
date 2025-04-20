#pragma once
#include "ObjectCore.h"
#include "Containers/Array.h"

namespace Lumina
{
    
    template<typename T>
    class TDeferredRegistry
    {
    public:
        
        using TType = typename T::TType;

        static TDeferredRegistry& Get()
        {
            static TDeferredRegistry Registry;
            return Registry;
        }

        struct FRegistrant
        {
            TType* (*RegisterFunc)();
        };


        void AddRegistration(TType* (*RegisterFunc)())
        {
            Registrations.emplace_back(RegisterFunc);
        }

        void ProcessRegistrations()
        {
            uint32 Num = Registrations.size();
            for (FRegistrant& Registrant : Registrations)
            {
                Registrant.RegisterFunc();    
            }

            ProcessedRegistrations = Num;
        }

        bool HasPendingRegistrations()
        {
            return Registrations.size() != ProcessedRegistrations;
        }

    private:
        
        TVector<FRegistrant>    Registrations;
        uint32                  ProcessedRegistrations = 0;
        
    };

    using FClassDeferredRegistry = TDeferredRegistry<FClassRegistrationInfo>;
}
