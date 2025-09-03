#pragma once

#include "Containers/Array.h"
#include "Core/Utils/NonCopyable.h"
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
        virtual void Initialize() = 0;
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
            size_t TypeHash = typeid(T).hash_code();
            Assert(SubsystemLookup.find(TypeHash) == SubsystemLookup.end())

            T* pSubsystem = Memory::New<T>(std::forward<Args>(args)...);
            
            SubsystemLookup.emplace(TypeHash, pSubsystem);
            
            pSubsystem->Initialize();
            LOG_TRACE("Subsystems: Created Type: {0}", typeid(T).name());
            return pSubsystem;
        }

        template<typename T>
        requires std::is_base_of_v<ISubsystem, T>
        void RemoveSubsystem()
        {
            size_t TypeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(TypeHash);
            if (it != SubsystemLookup.end())
            {
                ISubsystem* pSubsystem = it->second;
            
                pSubsystem->Deinitialize();
                Memory::Delete(pSubsystem);
                SubsystemLookup.erase(TypeHash);

                LOG_TRACE("Subsystems: Removed Type: {0}", typeid(T).name());
            }
        }

        template<typename T>
        requires std::is_base_of_v<ISubsystem, T>
        T* GetSubsystem()
        {
            size_t TypeHash = typeid(T).hash_code();
            auto it = SubsystemLookup.find(TypeHash);
            return static_cast<T*>(it->second);
        }

    private:
        
        THashMap<size_t, ISubsystem*> SubsystemLookup;
    };
}
