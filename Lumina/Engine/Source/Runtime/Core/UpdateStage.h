#pragma once

#include <cstring> // For memset
#include "Platform/GenericPlatform.h"

namespace Lumina
{
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

    enum class EUpdatePriority : uint8
    {
        Highest     = 0,
        High        = 64,
        Medium      = 128,
        Low         = 192,
        Disabled    = 255,
        Default     = Medium,
    };

    struct FUpdateStagePriority
    {
        inline FUpdateStagePriority(EUpdateStage InStage) : Stage(InStage) { }
        inline FUpdateStagePriority(EUpdateStage InStage, EUpdatePriority InPriority) : Stage(InStage), Priority(InPriority) { }

    public:

        EUpdateStage     Stage;
        EUpdatePriority  Priority = EUpdatePriority::Default;
    };

    using RequiresUpdate = FUpdateStagePriority;
    
    struct FUpdatePriorityList
    {
        FUpdatePriorityList()
        {
            Reset();
        }

        template<typename... Args>
        FUpdatePriorityList( Args&&... args )
        {
            Reset();
            ((*this << static_cast<Args&&>(args)), ...);
        }

        inline void Reset()
        {
            memset(Priorities, (uint8) EUpdatePriority::Disabled, sizeof(Priorities));
        }

        inline bool IsStageEnabled(EUpdateStage stage) const
        {
            return Priorities[(uint8) stage] != (uint8) EUpdatePriority::Disabled;
        }

        inline uint8 GetPriorityForStage(EUpdateStage stage) const
        {
            return Priorities[(uint8) stage];
        }

        inline FUpdatePriorityList& SetStagePriority( FUpdateStagePriority&& stagePriority )
        {
            Priorities[(uint8) stagePriority.Stage] = (uint8)stagePriority.Priority;
            return *this;
        }

        // Set a priority for a given stage
        inline FUpdatePriorityList& operator<<(FUpdateStagePriority&& stagePriority)
        {
            Priorities[(uint8) stagePriority.Stage] = (uint8)stagePriority.Priority;
            return *this;
        }

        inline bool AreAllStagesDisabled() const
        {
            static uint8 const disabledStages[(uint8) EUpdateStage::Max] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            static_assert( sizeof( disabledStages ) == sizeof(Priorities), "disabled stages must be the same size as the priorities list" );
            return memcmp(Priorities, disabledStages, sizeof(Priorities)) == 0;
        }

    private:

        uint8           Priorities[(uint8)EUpdateStage::Max];
    };
}
