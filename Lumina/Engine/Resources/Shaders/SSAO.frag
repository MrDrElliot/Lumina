#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

layout(set = 1, binding = 0) uniform sampler2D uPositionDepth;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uNoise;

const int SSAO_KERNEL_SIZE = 64;
const float SSAO_RADIUS = 0.3;

layout(set = 1, binding = 3) uniform SSBOKernal
{
    vec4 Samples[SSAO_KERNEL_SIZE];
} uSSAOKernal;

layout(location = 0) in vec2 inUV;

layout(location = 0) out float outFragColor;

void main()
{
    // Reconstruct view-space position from world-space
    vec3 WorldPos = texture(uPositionDepth, inUV).rgb;
    vec3 FragPos = WorldToView(WorldPos);

    // Get view-space normal
    vec3 WorldNormal = texture(uNormal, inUV).rgb * 2.0 - 1.0;
    vec3 ViewNormal = normalize(NormalWorldToView(WorldNormal));

    // Generate noise-based random vector in tangent space
    ivec2 screenSize = textureSize(uPositionDepth, 0);
    ivec2 noiseSize  = textureSize(uNoise, 0);
    vec2 noiseUV = inUV * vec2(screenSize) / vec2(noiseSize);
    vec3 randomVec = texture(uNoise, noiseUV).xyz * 2.0 - 1.0;

    // Construct TBN matrix in view space
    vec3 tangent   = normalize(randomVec - ViewNormal * dot(randomVec, ViewNormal));
    vec3 bitangent = normalize(cross(tangent, ViewNormal));
    mat3 TBN       = mat3(tangent, bitangent, ViewNormal);

    
    float occlusion = 0.0f;
    const float bias = 0.025f;
    
    for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
    {
        vec3 samplePos = TBN * uSSAOKernal.Samples[i].xyz;
        samplePos = FragPos + samplePos * SSAO_RADIUS;

        // project
        vec4 offset = vec4(samplePos, 1.0f);
        offset = GetCameraProjection() * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;

        float sampleDepth = -texture(uPositionDepth, offset.xy).w;

        float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(FragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));

    outFragColor = occlusion;
}
