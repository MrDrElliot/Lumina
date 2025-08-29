#pragma once

#include "Component.h"
#include "glm/glm.hpp"
#include "Core/Math/Transform.h"
#include "TransformComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API STransformComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(STransformComponent);
        
        STransformComponent() = default;
        STransformComponent(const FTransform& InTransform)
            : Transform(InTransform)
        {}
        
        STransformComponent(const glm::vec3& InPosition)
            :Transform(InPosition)
        {}
        
        FTransform& GetTransform() { return Transform; }
        void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

        glm::vec3 GetLocation() const    { return Transform.Location; }
        glm::quat GetRotation() const    { return Transform.Rotation; }
        glm::vec3 GetScale()    const    { return Transform.Scale; }
        glm::mat4 GetMatrix()   const    { return CachedMatrix; }
        
        void SetLocation(const glm::vec3& InLocation) 
        { 
            Transform.Location = InLocation; 
        }

        void SetRotation(const glm::quat& InRotation) 
        { 
            Transform.Rotation = InRotation; 
        }

        void SetRotationFromEuler(const glm::vec3& EulerRotation)
        {
            Transform.Rotation = glm::quat(glm::radians(EulerRotation));
        }

        void SetScale(const glm::vec3& InScale) 
        { 
            Transform.Scale = InScale; 
        }

        glm::vec3 GetRotationAsEuler() const 
        {
            return glm::degrees(glm::eulerAngles(Transform.Rotation));
        }

    public:

        LUM_PROPERTY(Editable, Category = "Transform")
        FTransform Transform;
        
        FTransform WorldTransform = Transform;
        
        glm::mat4 CachedMatrix = WorldTransform.GetMatrix();
    };
    
    struct FDirtyTransform { };

}
