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
layout(location = 2) out vec3 outFragPos;
layout(location = 3) out vec2 outUV;
layout(location = 4) out mat4 inModelMatrix;

precise invariant gl_Position;

void main()
{
    mat4 model = GetModelMatrix(gl_InstanceIndex);
    inModelMatrix = model;

    vec4 WorldPos = model * vec4(inPosition.xyz, 1.0);
    outFragPos = WorldPos.xyz;

    mat3 NormalMatrix = transpose(inverse(mat3(model)));
    outNormal = normalize(NormalMatrix * inNormal.xyz);

    outUV = vec2(inUV.x, 1.0 - inUV.y);

    outFragColor = inColor;

    gl_Position = GetCameraProjection() * GetCameraView() * WorldPos;
}
