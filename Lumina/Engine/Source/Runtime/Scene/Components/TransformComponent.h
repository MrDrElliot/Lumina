#pragma once
#include "Math/Transform.h"

namespace Lumina
{
    class FTransformComponent : public FEntityComponent
    {
    public:
        // Constructor: Initializes with a default or provided transform
        FTransformComponent(const FTransform& InTransform = FTransform()) : Transform(InTransform) {}

    
        // Getter and setter methods for the full transform
        inline FTransform& GetTransform() { return Transform; }
        inline void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

        // Getter methods for position, rotation, and scale
        inline glm::vec3 GetLocation() const    { return Transform.Location; }
        inline glm::quat GetRotation() const    { return Transform.Rotation; }
        inline glm::vec3 GetScale()    const    { return Transform.Scale; }
    
        // Setter methods for location, rotation, and scale
        // These ensure the rotation is applied correctly (typically in quaternions) and maintain consistency in transform calculations

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
            // Converts Euler angles (in degrees or radians) to a quaternion
            Transform.Rotation = glm::quat(glm::radians(EulerRotation));
        }

        FORCEINLINE void SetScale(const glm::vec3& InScale) 
        { 
            Transform.Scale = InScale; 
        }

        // Helper function to convert rotation from Euler angles (degrees)
        FORCEINLINE glm::vec3 GetRotationAsEuler() const 
        {
            // Convert the stored quaternion back to Euler angles for easier manipulation/display
            return glm::degrees(glm::eulerAngles(Transform.Rotation));
        }

        // Store the transform (position, rotation, scale)
        FTransform Transform;
    };

}