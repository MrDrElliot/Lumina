#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

// Uniform data
layout(set = 0, binding = 0) uniform UBO
{
    float time;
};

// Output color
layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outFragColor;


void main()
{
    outFragColor = vec4(inColor, 1.0);
}
