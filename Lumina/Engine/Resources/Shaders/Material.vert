#version 460 core
#extension GL_ARB_shader_draw_parameters : enable

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

// Input attributes
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec4 inNormal;
layout(location = 3) in vec2 inUV;

// Outputs
layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outFragPos;
layout(location = 3) out vec2 outUV;

precise invariant gl_Position;

void main()
{
    outUV = inUV;
    
    mat4 ModelMatrix = GetModelMatrix(gl_InstanceIndex);
    
    mat4 View = GetCameraView();
    vec4 ViewPosition = View * ModelMatrix * vec4(inPosition.xyz, 1.0);
    outFragPos = ViewPosition;
    
    vec4 WorldPos = ModelMatrix * vec4(inPosition.xyz, 1.0);

    mat3 NormalMatrix = transpose(inverse(mat3(View * ModelMatrix)));
    outNormal = NormalMatrix * inNormal.xyz;


    outFragColor = inColor;
    
    gl_Position = GetCameraProjection() * ViewPosition;
}
