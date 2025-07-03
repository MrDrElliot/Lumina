#version 450
#pragma shader_stage(vertex)

#include "Includes/Primitives.glsl"


struct FCameraView
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
};

layout(set = 0, binding = 0) readonly uniform SceneGlobals
{
    FCameraView CameraView;
    float Time;
    float DeltaTime;
} SceneUBO;

float GetTime()
{
    return SceneUBO.Time;
}

float GetDeltaTime()
{
    return SceneUBO.DeltaTime;
}

vec3 GetCameraPosition()
{
    // Disregard forth element, it was only used for padding.
    return SceneUBO.CameraView.CameraPosition.xyx;
}

mat4 GetCameraView()
{
    return SceneUBO.CameraView.CameraView;
}

mat4 GetCameraProjection()
{
    return SceneUBO.CameraView.CameraProjection;
}

vec3 WorldToView(vec3 worldPos)
{
    return (GetCameraView() * vec4(worldPos, 1.0)).xyz;
}

vec4 ViewToClip(vec3 viewPos)
{
    return GetCameraProjection() * vec4(viewPos, 1.0);
}

vec4 WorldToClip(vec3 worldPos)
{
    return GetCameraProjection() * GetCameraView() * vec4(worldPos, 1.0);
}

float SineWave(float speed, float amplitude)
{
    return amplitude * sin(float(GetTime()) * speed);
}


layout(location = 0) out vec3 fragNormal;

void main()
{
    vec3 Pos = CubeVertices[gl_VertexIndex];

    mat4 View = mat4(mat3(GetCameraView()));
    gl_Position = GetCameraProjection() * View * vec4(Pos, 1.0);
    
    fragNormal = Pos;
}