#pragma once

#if _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4435; disable : 4996)
#endif

#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "glm/fwd.hpp"
#include "Memory/Memory.h"

#if _MSC_VER
#pragma warning(pop)
#endif

namespace Lumina::Physics
{
    struct LUMINA_API Constants
    {
        static constexpr float const    s_lengthScale = 1.0f;
        static constexpr float const    s_speedScale = 9.81f;
        static glm::vec3 const          s_gravity;
    };

    namespace PX
    {
        struct Conversion
        {
            static glm::quat const         s_capsuleConversionToPx;
            static glm::quat const         s_capsuleConversionFromPx;
        };

        struct Shapes
        {
            static physx::PxConvexMesh*     s_pUnitCylinderMesh;
        };

        class Allocator final : public physx::PxAllocatorCallback
        {
            virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override
            {
                return Memory::Malloc(size, 16);
            }

            virtual void deallocate( void* ptr ) override
            {
                Memory::Free(ptr);
            }
        };

        class UserErrorCallback final : public physx::PxErrorCallback
        {
            virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
            {
                if (code >= physx::PxErrorCode::eINVALID_PARAMETER)
                {
                    LUMINA_NO_ENTRY()
                }

                LOG_ERROR("[PhysX] - {} {} {}", message, file, line);
            }
        };
    }
    
    
    void Initialize();
    void Shutdown();
    physx::PxPhysics* GetPxPhysics();
}
