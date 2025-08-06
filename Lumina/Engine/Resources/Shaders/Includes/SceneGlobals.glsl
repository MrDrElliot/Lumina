
#define MAX_LIGHTS 1024

struct FCameraView
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
};

const uint LIGHT_TYPE_DIRECTIONAL = 0;
const uint LIGHT_TYPE_POINT       = 1;
const uint LIGHT_TYPE_SPOT        = 2;

struct FLight
{
    vec4 Position;      // xyz: position, w: range or falloff scale
    vec4 Direction;     // xyz: direction (normalized), w: inner cone cos angle
    vec4 Color;         // rgb: color * intensity, a: unused or padding
    vec2 ConeAngles;    // x: cos(inner cone), y: cos(outer cone)
    float Radius;       // Radius.
    uint Type;          // Type of the light
};

layout(set = 0, binding = 0) readonly uniform SceneGlobals
{
    FCameraView CameraView;
    float Time;
    float DeltaTime;
    float NearPlane;
    float FarPlane;
} SceneUBO;

layout(set = 0, binding = 1) readonly buffer FModelData
{
    mat4 ModelMatrix[]; // Raw flat buffer of scatterd models (not sorted).
} ModelData;


layout(set = 0, binding = 2) readonly buffer FLightData
{
    uint    NumLights;
    FLight  Lights[MAX_LIGHTS];
} LightData;




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

vec3 GetModelLocation(uint Index)
{
    return vec3(ModelData.ModelMatrix[Index][3].xyz);
}

mat4 GetModelMatrix(uint Index)
{
    return ModelData.ModelMatrix[Index];
}

vec3 WorldToView(vec3 worldPos)
{
    return (GetCameraView() * vec4(worldPos, 1.0)).xyz;
}

vec3 NormalWorldToView(vec3 Normal)
{
    return mat3(GetCameraView()) * Normal;
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

FLight GetLightAt(uint Index)
{
    return LightData.Lights[Index];
}

uint GetNumLights()
{
    return LightData.NumLights;
}

float LinearDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0 * SceneUBO.NearPlane * SceneUBO.FarPlane) / (SceneUBO.FarPlane - z * (SceneUBO.FarPlane - SceneUBO.NearPlane));
}