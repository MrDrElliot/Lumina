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
    outFragColor = 1.0f;
}
