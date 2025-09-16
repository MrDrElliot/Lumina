#pragma once
#include "UpdateStage.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FSubsystemManager;

    class LUMINA_API FUpdateContext
    {
    public:

        friend class FEngine;
        
        FORCEINLINE void MarkFrameEnd(double InTime)
        {
            DeltaTime = InTime - LastFrameTime;
            LastFrameTime = InTime;
            Frame++;
            Time = InTime;
        }
        
        FORCEINLINE double GetTime() const { return Time; }
        FORCEINLINE float GetFPS() const { return 1.0f / (float)DeltaTime; }
        FORCEINLINE double GetDeltaTime() const { return DeltaTime; }
        FORCEINLINE uint64 GetFrame() const { return Frame; }
        FORCEINLINE EUpdateStage GetUpdateStage() const { return UpdateStage; }
        
        FORCEINLINE FSubsystemManager* GetSubsystemManager() const { return SubsystemManager; }

        template<typename T>
        FORCEINLINE T* GetSubsystem() const { return SubsystemManager->GetSubsystem<T>(); }

        
    protected:

        double              Time = 0;
        double              DeltaTime = 60.0f;
        double              LastFrameTime = 0.0;
        float               FrameRateLimit = 144.0f;
        uint64              Frame = 0;
        EUpdateStage        UpdateStage = EUpdateStage::Max;
        FSubsystemManager*  SubsystemManager = nullptr;
    };
}
