#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vec4(inColor.rgb, 1.0);
}