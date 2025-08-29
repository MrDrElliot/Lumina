#include "Physics.h"
#include "Core/Assertions/Assert.h"
#include "Memory/Memory.h"

namespace Lumina::Physics::PX
{
    struct GlobalState
    {
        void Initialize()
        {
            LUM_ASSERT(Foundation == nullptr && Physics == nullptr)

            physx::PxTolerancesScale tolerancesScale;
            tolerancesScale.length = Constants::s_lengthScale;
            tolerancesScale.speed = Constants::s_speedScale;

            //-------------------------------------------------------------------------

            AllocatorCallback = Memory::New<Allocator>();
            ErrorCallback = Memory::New<UserErrorCallback>();

            Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *AllocatorCallback, *ErrorCallback);
            LUM_ASSERT(Foundation != nullptr)

            #if WITH_DEVELOPMENT_TOOLS
            m_pPVD = PxCreateOmniPvd( *m_pFoundation );
            m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_pFoundation, tolerancesScale, true, nullptr, m_pPVD );
            #else
            Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, tolerancesScale, true);
            #endif

        }

        void Shutdown()
        {
            LUM_ASSERT(Foundation != nullptr && Physics != nullptr);

            Physics->release();
            Foundation->release();

            Memory::Delete(ErrorCallback);
            Memory::Delete(AllocatorCallback);
        }

    public:

        physx::PxFoundation*                            Foundation = nullptr;
        physx::PxPhysics*                               Physics = nullptr;
        physx::PxAllocatorCallback*                     AllocatorCallback = nullptr;
        physx::PxErrorCallback*                         ErrorCallback = nullptr;
        physx::PxSimulationEventCallback*               EventCallbackHandler = nullptr;

        #if WITH_DEVELOPMENT_TOOLS
        physx::PxOmniPvd*                               m_pPVD;
        #endif
    };
}


namespace Lumina::Physics
{
    static PX::GlobalState* GGlobalState = nullptr;

    
    void Initialize()
    {
        LUM_ASSERT(GGlobalState == nullptr)

        GGlobalState = Memory::New<PX::GlobalState>();
        GGlobalState->Initialize();
    }

    void Shutdown()
    {
        LUM_ASSERT(GGlobalState != nullptr)

        GGlobalState->Shutdown();
        Memory::Delete(GGlobalState);
    }

    physx::PxPhysics* GetPxPhysics()
    {
        return GGlobalState->Physics;
    }
}
