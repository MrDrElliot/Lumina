#version 450 core
#pragma shader_stage(vertex)

#include "Includes/SceneGlobals.glsl"

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 outColor;


float time = GetTime();

void main()
{
    mat4 model = GetModelMatrix(gl_InstanceIndex);
    vec4 worldPos = model * vec4(inPosition.xyz, 1.0);

    gl_Position = GetCameraProjection() * GetCameraView() * worldPos;
    outColor = inColor.xyz;
}
