#version 450 core

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

//------------------------------------------------------------------------------------------------------------
// Vertex Shader

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main()
{
    gl_Position = vec4(inPosition * 0.5, sin(GetTime()));
    fragColor = inColor;
}


#endif



//------------------------------------------------------------------------------------------------------------
// Fragment Shader

#ifdef FRAGMENT_SHADER

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = fragColor; 
}

#endif
