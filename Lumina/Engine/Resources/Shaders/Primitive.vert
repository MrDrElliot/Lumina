#version 450 core

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"


layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position = inPosition;
    outColor = inColor.rgb;
}