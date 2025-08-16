#pragma once
#include "RenderComponent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Lumina
{
    struct FBatchedLine
    {
        glm::vec3 Start;

        glm::vec3 End;

        glm::vec4 Color = glm::vec4(1.0f);

        float Thickness = 1.0f;

        float Duration = 0.0f;
    };
    
    struct FLineBatcherComponent : SRenderComponent
    {
        TFixedVector<FBatchedLine, 2024> BatchedLines;

        LUMINA_API void DrawLine(const glm::vec3& Start, const glm::vec3& End, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f)
        {
            FBatchedLine Line;
            Line.Start = Start;
            Line.End = End;
            Line.Color = Color;
            Line.Thickness = Thickness;
            Line.Duration = Duration;
            BatchedLines.push_back(Line);
        }

        LUMINA_API void DrawBox(const glm::vec3& Center, const glm::vec3& Extents, const glm::quat& Rotation, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f)
        {
            BatchedLines.reserve(BatchedLines.size() + 12);

            glm::vec3 LocalCorners[8] =
            {
                {-Extents.x, -Extents.y, -Extents.z},
                { Extents.x, -Extents.y, -Extents.z},
                { Extents.x,  Extents.y, -Extents.z},
                {-Extents.x,  Extents.y, -Extents.z},

                {-Extents.x, -Extents.y,  Extents.z},
                { Extents.x, -Extents.y,  Extents.z},
                { Extents.x,  Extents.y,  Extents.z},
                {-Extents.x,  Extents.y,  Extents.z},
            };

            glm::vec3 corners[8];
            for (int i = 0; i < 8; ++i)
            {
                corners[i] = Center + glm::rotate(Rotation, LocalCorners[i]);
            }

            // Bottom face edges
            DrawLine(corners[0], corners[1], Color, Thickness, Duration);
            DrawLine(corners[1], corners[2], Color, Thickness, Duration);
            DrawLine(corners[2], corners[3], Color, Thickness, Duration);
            DrawLine(corners[3], corners[0], Color, Thickness, Duration);

            // Top face edges
            DrawLine(corners[4], corners[5], Color, Thickness, Duration);
            DrawLine(corners[5], corners[6], Color, Thickness, Duration);
            DrawLine(corners[6], corners[7], Color, Thickness, Duration);
            DrawLine(corners[7], corners[4], Color, Thickness, Duration);

            // Vertical edges
            DrawLine(corners[0], corners[4], Color, Thickness, Duration);
            DrawLine(corners[1], corners[5], Color, Thickness, Duration);
            DrawLine(corners[2], corners[6], Color, Thickness, Duration);
            DrawLine(corners[3], corners[7], Color, Thickness, Duration);
        }

        LUMINA_API void DrawSphere(const glm::vec3& Center, float Radius, const glm::vec4& Color, uint8 Segments = 16, float Thickness = 1.0f, float Duration = 1.0f)
        {
            BatchedLines.reserve(BatchedLines.size() + (Segments - 1) * Segments + Segments * Segments);

            for (uint8 lat = 1; lat < Segments; ++lat)
            {
                float latAngle = glm::pi<float>() * lat / Segments;
                float y = Radius * cos(latAngle);
                float ringRadius = Radius * sin(latAngle);

                for (int lon = 0; lon < Segments; ++lon)
                {
                    float lonAngle1 = glm::two_pi<float>() * lon / Segments;
                    float lonAngle2 = glm::two_pi<float>() * (lon + 1) / Segments;

                    glm::vec3 p1 = Center + glm::vec3(ringRadius * cos(lonAngle1), y, ringRadius * sin(lonAngle1));
                    glm::vec3 p2 = Center + glm::vec3(ringRadius * cos(lonAngle2), y, ringRadius * sin(lonAngle2));

                    DrawLine(p1, p2, Color, Thickness, Duration);
                }
            }

            for (uint8 lon = 0; lon < Segments; ++lon)
            {
                float lonAngle = glm::two_pi<float>() * lon / Segments;

                for (int lat = 0; lat < Segments; ++lat)
                {
                    float latAngle1 = glm::pi<float>() * lat / Segments;
                    float latAngle2 = glm::pi<float>() * (lat + 1) / Segments;

                    glm::vec3 p1 = Center + glm::vec3(Radius * sin(latAngle1) * cos(lonAngle),
                                                      Radius * cos(latAngle1),
                                                      Radius * sin(latAngle1) * sin(lonAngle));

                    glm::vec3 p2 = Center + glm::vec3(Radius * sin(latAngle2) * cos(lonAngle),
                                                      Radius * cos(latAngle2),
                                                      Radius * sin(latAngle2) * sin(lonAngle));

                    DrawLine(p1, p2, Color, Thickness, Duration);
                }
            }
        }

        LUMINA_API void DrawCone(const glm::vec3& Apex, const glm::vec3& Direction, float AngleRadians, float Length, const glm::vec4& Color,
                                             uint8 Segments = 16, uint8 Stacks = 4,
                                             float Thickness = 1.0f, float Duration = 1.0f)
        {
            glm::vec3 dir = glm::normalize(Direction);
        
            glm::vec3 up = glm::abs(dir.y) < 0.99f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
            glm::vec3 right = glm::normalize(glm::cross(dir, up));
            glm::vec3 coneUp = glm::normalize(glm::cross(right, dir));
        
            for (uint8 stack = 1; stack <= Stacks; ++stack)
            {
                float t = (float)stack / Stacks;          // 0..1 along cone length
                float ringLength = t * Length;            // distance from apex
                float ringRadius = ringLength * tan(AngleRadians);
        
                TVector<glm::vec3> circlePoints(Segments);
                for (int i = 0; i < Segments; ++i)
                {
                    float theta = glm::two_pi<float>() * i / Segments;
                    circlePoints[i] = Apex + dir * ringLength + ringRadius * (cos(theta) * right + sin(theta) * coneUp);
                }
        
                for (uint8 i = 0; i < Segments; ++i)
                {
                    DrawLine(circlePoints[i], circlePoints[(i + 1) % Segments], Color, Thickness, Duration);
                }
        
                for (uint8 i = 0; i < Segments; ++i)
                {
                    DrawLine(Apex, circlePoints[i], Color, Thickness, Duration);
                }
        
                if (stack > 1)
                {
                    float tPrev = (float)(stack - 1) / Stacks;
                    float prevLength = tPrev * Length;
                    float prevRadius = prevLength * tan(AngleRadians);
        
                    for (int i = 0; i < Segments; ++i)
                    {
                        glm::vec3 prevPoint = Apex + dir * prevLength + prevRadius * (cos(2*glm::pi<float>() * i / Segments) * right +
                                                                                   sin(2*glm::pi<float>() * i / Segments) * coneUp);
                        DrawLine(prevPoint, circlePoints[i], Color, Thickness, Duration);
                    }
                }
            }
        }


        LUMINA_API void Flush()
        {
            BatchedLines.clear();
        }
    };
}
