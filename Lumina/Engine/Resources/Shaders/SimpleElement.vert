#version 450 core

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"

// Input attributes
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform CameraPC
{
    mat4 CameraViewProjection;
} PC;

void main()
{
    outColor = inColor;
    
    gl_Position = PC.CameraViewProjection * inPosition;
}