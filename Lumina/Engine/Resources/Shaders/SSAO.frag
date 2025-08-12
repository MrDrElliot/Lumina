#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

layout(set = 1, binding = 0) uniform sampler2D uPositionDepth;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uNoise;

const int SSAO_KERNEL_SIZE = 32;
const float SSAO_RADIUS = 0.3;

layout(set = 1, binding = 3) uniform SSBOKernal
{
    vec4 Samples[SSAO_KERNEL_SIZE];
} uSSAOKernal;

layout(location = 0) in vec2 inUV;

layout(location = 0) out float outFragColor;

void main()
{
    // FragPos and Normal are in *view-space*.
    vec3 FragPos = texture(uPositionDepth, inUV).rgb;
    vec3 Normal = normalize(texture(uNormal, inUV).rgb) * 2.0 - 1.0;
    
    ivec2 TexDim = textureSize(uPositionDepth, 0);
    ivec2 NoiseDim = textureSize(uNoise, 0);
    
    const vec2 NoiseUV = vec2(float(TexDim.x) / float(NoiseDim.x), float(TexDim.y) / float(NoiseDim.y)) * inUV;
    vec3 RandomVec = texture(uNoise, NoiseUV).xyz * 2.0 - 1.0;
    
    vec3 Tangent = normalize(RandomVec - Normal * dot(RandomVec, Normal));
    vec3 BiTangent = cross(Tangent, Normal);
    mat3 TBN = mat3(Tangent, BiTangent, Normal);
    
    float Occlusion = 0.0f;
    const float Bias = 0.025f;
    
    for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
    {
        // Get sample position.
        vec3 SamplerPos = TBN * uSSAOKernal.Samples[i].xyz; // Tangent-Space to View-Space
        SamplerPos = FragPos + SamplerPos * SSAO_RADIUS;
        
        // Get sample position in screen space.
        vec4 Offset = vec4(SamplerPos, 1.0f);
        Offset = GetCameraProjection() * Offset; // From View-Space to Clip-Space.
        Offset.xy /= Offset.w; // Perspective divide.
        Offset.xy = Offset.xy * 0.5f + 0.5f; // Transform to range 0.0 - 1.0
        Offset.y = 1.0 - Offset.y;

        float SampleDepth = texture(uPositionDepth, Offset.xy).z;
        
        float RangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(FragPos.z - SampleDepth));
        
        // In view-space, greater Z values are closer to the camera.
        Occlusion += (SampleDepth >= SamplerPos.z - Bias ? 1.0f : 0.0f) * RangeCheck;
    }
    
    float Power = 1.5f;
    Occlusion = 1.0 - (Occlusion / float(SSAO_KERNEL_SIZE));
    outFragColor = pow(Occlusion, Power);
}
