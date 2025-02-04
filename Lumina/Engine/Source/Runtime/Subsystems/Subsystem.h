#pragma once

#include "Containers/Array.h"
#include "EASTL/sort.h"
#include "Log/Log.h"

namespace Lumina
{
    class ISubsystem
    {
    public:

        friend class FSubsystemManager;
        
        virtual ~ISubsystem() = default;
        virtual void Initialize() = 0;
        virtual void Update(double DeltaTime) { }
        virtual void Deinitialize() = 0;

    private:

    };

    class FSubsystemManager
    {
    public:
        
        ~FSubsystemManager() { DeinitializeAll(); }

        template<typename T, typename... Args>
        T* AddSubsystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem!");

            uint32_t typeHash = typeid(T).hash_code();
            RemoveSubsystem<T>();

            T* pSubsystem = new T(std::forward<Args>(args)...);
            
            FlatUpdateList.push_back(pSubsystem);
            SubsystemLookup[typeHash] = pSubsystem;
            

            pSubsystem->Initialize();
            LOG_TRACE("Subsystems: Created Type: {0}", typeid(T).name());
            return pSubsystem;
        }

        template<typename T>
        void RemoveSubsystem()
        {
            uint32_t typeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            if (it != SubsystemLookup.end())
            {
                ISubsystem* pSubsystem = it->second;

                FlatUpdateList.erase(std::remove(FlatUpdateList.begin(), FlatUpdateList.end(), pSubsystem), FlatUpdateList.end());

                delete pSubsystem;
                SubsystemLookup.erase(typeHash);

                LOG_TRACE("Subsystems: Removed Type: {0}", typeid(T).name());
            }
        }

        template<typename T>
        T* GetSubsystem()
        {
            uint32_t typeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            return (it != SubsystemLookup.end()) ? static_cast<T*>(it->second) : nullptr;
        }

        void DeinitializeAll()
        {
            for (ISubsystem* subsystem : FlatUpdateList)
            {
                subsystem->Deinitialize();
                delete subsystem;
            }
            FlatUpdateList.clear();
            SubsystemLookup.clear();
            LOG_TRACE("Subsystems: All deinitialized");
        }

    private:
        
        TVector<ISubsystem*> FlatUpdateList;
        THashMap<uint32, ISubsystem*> SubsystemLookup;
    };
}
