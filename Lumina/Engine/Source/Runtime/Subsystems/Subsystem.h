#pragma once

#include "Containers/Array.h"
#include "Core/Utils/NonCopyable.h"
#include "EASTL/sort.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FUpdateContext;

    class LUMINA_API ISubsystem : public INonCopyable
    {
    public:

        friend class FSubsystemManager;
        
        virtual ~ISubsystem() = default;
        virtual void Initialize(FSubsystemManager& Manager) = 0;
        virtual void Deinitialize() = 0;

    private:

    };

    class LUMINA_API FSubsystemManager
    {
    public:
        
        ~FSubsystemManager()
        {
            Assert(SubsystemLookup.empty())
        }

        template<typename T, typename... Args>
        requires std::is_base_of_v<ISubsystem, T>
        T* AddSubsystem(Args&&... args)
        {
            uint32 typeHash = (uint32)typeid(T).hash_code();
            Assert(GetSubsystem<T>() == nullptr);

            T* pSubsystem = Memory::New<T>(std::forward<Args>(args)...);
            
            SubsystemLookup.insert_or_assign(typeHash, pSubsystem);
            
            pSubsystem->Initialize(*this);
            LOG_TRACE("Subsystems: Created Type: {0}", typeid(T).name());
            return pSubsystem;
        }

        template<typename T>
        requires std::is_base_of_v<ISubsystem, T>
        void RemoveSubsystem()
        {
            uint32 typeHash = (uint32)typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            if (it != SubsystemLookup.end())
            {
                ISubsystem* pSubsystem = it->second;
            
                pSubsystem->Deinitialize();
                Memory::Delete(pSubsystem);
                SubsystemLookup.erase(typeHash);

                LOG_TRACE("Subsystems: Removed Type: {0}", typeid(T).name());
            }
        }

        template<typename T>
        requires std::is_base_of_v<ISubsystem, T>
        T* GetSubsystem()
        {
            uint32 typeHash = (uint32)typeid(T).hash_code();
            auto it = SubsystemLookup.find(typeHash);
            return (it != SubsystemLookup.end()) ? static_cast<T*>(it->second) : nullptr;
        }

    private:
        
        TVector<ISubsystem*> FlatUpdateList;
        THashMap<uint32, ISubsystem*> SubsystemLookup;
    };
}
