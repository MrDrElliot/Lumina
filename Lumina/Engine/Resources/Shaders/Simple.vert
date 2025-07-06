#version 450 core

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"
#include "Includes/Primitives.glsl"


struct FCameraView
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
};

layout(location = 0) out vec3 outColor;


layout(set = 0, binding = 0) readonly uniform FSceneGlobals
{
    FCameraView CameraView;
    float Time;
    float DeltaTime;
} SceneUBO;

layout(push_constant) uniform PushConstants
{
    mat4 ModelMatrix;
    vec4 Color;
} PC;

void main()
{
    vec3 Pos = CubeVertices[gl_VertexIndex];
    vec4 WorldPos = PC.ModelMatrix * vec4(Pos, 1.0);
    mat4 View = SceneUBO.CameraView.CameraView;
    mat4 Proj = SceneUBO.CameraView.CameraProjection;
    
    gl_Position = Proj * View * WorldPos;
    outColor = PC.Color.rgb;
}