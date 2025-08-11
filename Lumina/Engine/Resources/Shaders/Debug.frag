#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

layout(set = 0, binding = 0) uniform sampler2D uRenderTarget;
layout(set = 0, binding = 1) uniform sampler2D uPositionDepth;
layout(set = 0, binding = 2) uniform sampler2D uNormal;
layout(set = 0, binding = 3) uniform sampler2D uAlbedo;
layout(set = 0, binding = 4) uniform sampler2D uSSAO;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 OutFragColor;

#define NONE    0u
#define POSITION  (1u << 0)
#define NORMALS   (1u << 1)
#define ALBEDO    (1u << 2)
#define SSAO      (1u << 3)

layout(push_constant) uniform DebugInfo
{
    uint DebugFlags;
} Debug;

void main()
{
    vec4 baseColor = texture(uRenderTarget, inUV);
    OutFragColor = baseColor;

    if (HasFlag(Debug.DebugFlags, POSITION))
    {
        vec4 pos = texture(uPositionDepth, inUV);

        vec3 normalizedPos = clamp(pos.xyz * 0.1 + 0.5, 0.0, 1.0);
        OutFragColor = vec4(normalizedPos, 1.0);
    }

    else if (HasFlag(Debug.DebugFlags, NORMALS))
    {
        vec3 normal = texture(uNormal, inUV).xyz;
        normal = normalize(normal);
        vec3 normalColor = normal * 0.5 + 0.5;
        OutFragColor = vec4(normalColor, 1.0);
    }

    else if (HasFlag(Debug.DebugFlags, ALBEDO))
    {
        OutFragColor = texture(uAlbedo, inUV);
    }

    else if (HasFlag(Debug.DebugFlags, SSAO))
    {
        float ssao = texture(uSSAO, inUV).r;
        OutFragColor = vec4(vec3(ssao), 1.0);
    }
}
