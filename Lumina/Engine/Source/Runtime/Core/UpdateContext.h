#pragma once
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FSubsystemManager;

    class FUpdateContext
    {
    public:

        friend class FEngine;

        enum class EUpdateStage : uint8
        {
            FrameStart,
            PrePhysics,
            DuringPhysics,
            PostPhysics,
            FrameEnd,
            Paused,
            Max,
        };

        FORCEINLINE void MarkFrameStart()
        {
            UpdateStage = EUpdateStage::FrameStart;
        }
        FORCEINLINE void MarkFrameEnd(double Time)
        {
            DeltaTime = Time - LastFrameTime;
            LastFrameTime = Time;
            Frame++;
            UpdateStage = EUpdateStage::FrameEnd;
        }

        FORCEINLINE void UpdateDeltaTime(double InDelta) { DeltaTime = InDelta; }

        FORCEINLINE float GetFPS() const { return 1000.0f / DeltaTime; }
        FORCEINLINE double GetDeltaTime() const { return DeltaTime; }
        FORCEINLINE uint64 GetFrame() const { return Frame; }
        FORCEINLINE EUpdateStage GetUpdateStage() const { return UpdateStage; }
        
        
        FORCEINLINE FSubsystemManager* GetSubsystemManager() const { return SubsystemManager; }

        template<typename T>
        FORCEINLINE T* GetSubsystem() const { return SubsystemManager->GetSubsystem<T>(); }

    protected:

        double              DeltaTime = 1.0f / 60.0f;
        double              LastFrameTime = 0.0;
        float               FrameRateLimit = 144.0f;
        uint64              Frame = 0;
        EUpdateStage        UpdateStage = EUpdateStage::Max;
        FSubsystemManager*  SubsystemManager = nullptr;
    };
}
