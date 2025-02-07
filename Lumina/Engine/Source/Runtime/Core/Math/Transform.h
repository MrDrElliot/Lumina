#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>  // For quaternions

namespace Lumina
{
    class FTransform
    {
    public:
    
        glm::vec3 Location;   // Position in world space
        glm::quat Rotation;   // Rotation as a quaternion (no gimbal lock)
        glm::vec3 Scale;      // Scaling factors

        FTransform()
            : Location(0.0f),
              Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
              Scale(1.0f, 1.0f, 1.0f) {}

    
        glm::mat4 GetMatrix() const
        {
            // First, create scale, then rotation, then translation matrices
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);          // Scale
            glm::mat4 rotateMatrix = glm::mat4_cast(Rotation);  // Convert quaternion to rotation matrix
            glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), Location);  // Translate

            // Combine them into a single transformation matrix in the correct order
            return translateMatrix * rotateMatrix * scaleMatrix;
        }


        // Convenience methods to set rotation using Euler angles
        void SetRotationFromEuler(const glm::vec3& eulerAngles)
        {
            Rotation = glm::quat(glm::radians(eulerAngles));  // Convert Euler angles to quaternion
        }

        // Methods for manipulating the transform
        void Translate(const glm::vec3& translation)
        {
            Location += translation;  // Apply translation incrementally
        }

        void Rotate(const glm::vec3& eulerAngles)
        {
            glm::quat additionalRotation = glm::quat(glm::radians(eulerAngles));  // Convert Euler to quaternion
            Rotation = additionalRotation * Rotation;  // Combine existing and new rotations
        }

        void SetScale(const glm::vec3& scaleFactors)
        {
            Scale *= scaleFactors;  // Apply scaling incrementally
        }
    };
}