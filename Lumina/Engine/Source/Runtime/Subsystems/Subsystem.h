#pragma once

#include <typeindex>
#include <memory>
#include <EASTL/unordered_map.h>

#include "Memory/SmartPtr.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Log/Log.h"

enum ESubsystemFlags
{
    Flag_ManualShutdown,
};

namespace Lumina
{
    class ISubsystem
    {
    public:
        virtual ~ISubsystem() = default;
        virtual void Initialize() = 0;
        virtual void Update(double DeltaTime) { }
        virtual void Deinitialize() = 0;

    private:
        uint32_t Flags = 0; // Ensure you have the right type for your flags.
    };

    class SubsystemManager
    {
    public:
        ~SubsystemManager() = default;

        template<typename T, int Priority = 1, typename... Args>
        T* AddSubsystem(Args&&... args)
        {
            static_assert(std::is_base_of<ISubsystem, T>::value, "T must inherit from ISubsystem!");

            auto subsystem = MakeUniquePtr<T>(std::forward<Args>(args)...);
            T* pSubsystem = subsystem.get();
            Subsystems[typeid(T).hash_code()] = std::move(subsystem);
            pSubsystem->Initialize();

            LOG_TRACE("Subsystems: Creating Type: {0}", typeid(T).name());
            return pSubsystem;
        }

        template<typename T>
        T* GetSubsystem()
        {
            auto it = Subsystems.find(typeid(T).hash_code());
            if (it != Subsystems.end())
            {
                return static_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        void DeinitializeAll()
        {
            for (auto& [type, S] : Subsystems)
            {
                LOG_TRACE("Subsystems: Deinitializing Type: {0}", std::to_string(type));
                S->Deinitialize();
                S = nullptr;
            }
        }

    private:
        
        eastl::unordered_map<uint32, TUniquePtr<ISubsystem>> Subsystems;
    };
}
