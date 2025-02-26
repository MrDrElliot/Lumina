#pragma once

#include "Containers/Array.h"
#include "Core/Utils/NonCopyable.h"
#include "EASTL/sort.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FUpdateContext;

    class ISubsystem : public INonCopyable
    {
    public:

        friend class FSubsystemManager;
        
        virtual ~ISubsystem() = default;
        virtual void Initialize(FSubsystemManager& Manager) = 0;
        virtual void Deinitialize() = 0;

    private:

    };

    class FSubsystemManager
    {
    public:
        
        ~FSubsystemManager() { }

        template<typename T, typename... Args>
        T* AddSubsystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem!");

            uint32_t typeHash = typeid(T).hash_code();
            RemoveSubsystem<T>();

            T* pSubsystem = FMemory::New<T>(std::forward<Args>(args)...);
            
            FlatUpdateList.push_back(pSubsystem);
            SubsystemLookup[typeHash] = pSubsystem;
            

            pSubsystem->Initialize(*this);
            LOG_TRACE("Subsystems: Created Type: {0}", typeid(T).name());
            return pSubsystem;
        }

        template<typename T>
        void RemoveSubsystem()
        {
            static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem!");

            uint32_t typeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            if (it != SubsystemLookup.end())
            {
                ISubsystem* pSubsystem = it->second;
                FlatUpdateList.erase(std::remove(FlatUpdateList.begin(), FlatUpdateList.end(), pSubsystem), FlatUpdateList.end());
            
                pSubsystem->Deinitialize();
                FMemory::Delete(pSubsystem);
                SubsystemLookup.erase(typeHash);

                LOG_TRACE("Subsystems: Removed Type: {0}", typeid(T).name());
            }
        }

        template<typename T>
        T* GetSubsystem()
        {
            static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem!");

            uint32_t typeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            return (it != SubsystemLookup.end()) ? static_cast<T*>(it->second) : nullptr;
        }

    private:
        
        TVector<ISubsystem*> FlatUpdateList;
        THashMap<uint32, ISubsystem*> SubsystemLookup;
    };
}
