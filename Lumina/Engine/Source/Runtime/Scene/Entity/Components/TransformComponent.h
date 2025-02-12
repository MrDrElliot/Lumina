#pragma once

#include "Component.h"
#include "Core/Math/Transform.h"

namespace Lumina
{
    class FTransformComponent : public FEntityComponent
    {
    public:
        
        FTransformComponent(const FTransform& InTransform = FTransform()) : Transform(InTransform) {}

    
        inline FTransform& GetTransform() { return Transform; }
        inline void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

        inline glm::vec3 GetLocation() const    { return Transform.Location; }
        inline glm::quat GetRotation() const    { return Transform.Rotation; }
        inline glm::vec3 GetScale()    const    { return Transform.Scale; }
        

        FORCEINLINE void SetLocation(const glm::vec3& InLocation) 
        { 
            Transform.Location = InLocation; 
        }

        FORCEINLINE void SetRotation(const glm::quat& InRotation) 
        { 
            Transform.Rotation = InRotation; 
        }

        FORCEINLINE void SetRotationFromEuler(const glm::vec3& EulerRotation)
        {
            Transform.Rotation = glm::quat(glm::radians(EulerRotation));
        }

        FORCEINLINE void SetScale(const glm::vec3& InScale) 
        { 
            Transform.Scale = InScale; 
        }

        FORCEINLINE glm::vec3 GetRotationAsEuler() const 
        {
            return glm::degrees(glm::eulerAngles(Transform.Rotation));
        }

        FTransform Transform;
    };

}