#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>  // For quaternions

namespace Lumina
{
    class FTransform
    {
    public:
    
        glm::vec3 Location;
        glm::quat Rotation;
        glm::vec3 Scale;
        
        FTransform()
            : Location(0.0f),
              Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
              Scale(1.0f, 1.0f, 1.0f)
        {}

        FTransform(const glm::vec3& InPosition)
            : Location(InPosition),
              Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
              Scale(1.0f, 1.0f, 1.0f)
        {}

        FTransform(const glm::vec3& location, const glm::vec3& eulerAngles, const glm::vec3& scale)
            : Location(location),
              Rotation(glm::quat(glm::radians(eulerAngles))),
              Scale(scale)
        {
        }

        glm::mat4 GetMatrix() const
        {
            glm::mat4 m = glm::mat4_cast(Rotation);
            m[0] *= Scale.x;
            m[1] *= Scale.y;
            m[2] *= Scale.z;
            m[3] = glm::vec4(Location, 1.0f);

            return m;
        }

        void SetRotationFromEuler(const glm::vec3& eulerAngles)
        {
            Rotation = glm::quat(glm::radians(eulerAngles));
        }

        void Translate(const glm::vec3& translation)
        {
            Location += translation;
        }

        void Rotate(const glm::vec3& eulerAngles)
        {
            glm::quat additionalRotation = glm::quat(glm::radians(eulerAngles));
            Rotation = additionalRotation * Rotation;
        }

        void SetScale(const glm::vec3& scaleFactors)
        {
            Scale *= scaleFactors;
        }

        bool operator==(const FTransform& Other) const
        {
            return Location == Other.Location &&
                   Rotation == Other.Rotation &&
                   Scale == Other.Scale;
        }

        bool operator!=(const FTransform& Other) const
        {
            return !(*this == Other);
        }
    };
}

template <>
struct fmt::formatter<Lumina::FTransform>\
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Lumina::FTransform& transform, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), 
            "Location: ({:.2f}, {:.2f}, {:.2f}) | Rotation: ({:.2f}, {:.2f}, {:.2f}, {:.2f}) | Scale: ({:.2f}, {:.2f}, {:.2f})",
            transform.Location.x, transform.Location.y, transform.Location.z,
            transform.Rotation.w, transform.Rotation.x, transform.Rotation.y, transform.Rotation.z,
            transform.Scale.x, transform.Scale.y, transform.Scale.z);
    }
};
