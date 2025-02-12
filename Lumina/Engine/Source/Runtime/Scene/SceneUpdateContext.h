#pragma once
#include "Core/UpdateContext.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FScene;

    class FSceneUpdateContext : public FUpdateContext
    {
    public:

        FSceneUpdateContext(const FUpdateContext& ParentContext, FScene* Scene);
        

        FSceneUpdateContext(const FUpdateContext&) = delete;
        FSceneUpdateContext& operator=(const FUpdateContext&) = delete;
        
        FSubsystemManager* GetSceneSubsystemManager() const;
        
        template<typename T>
        FORCEINLINE T* GetSceneSubsystem() const
        {
            static_assert(std::is_base_of_v<ISubsystem, T>, "T must derive from ISubsystem");
            return GetSceneSubsystemManager()->GetSubsystem<T>();
        }
    
        
    private:
        
        FScene*         Scene = nullptr;
        
    };
}