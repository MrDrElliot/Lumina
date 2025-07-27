
struct FCameraView
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
};

struct FPointLight
{
    vec4 Position;
    vec4 Color;
};

struct FDirectionalLight
{
    vec4 Direction;
    vec4 Color;
};

layout(set = 0, binding = 0) readonly uniform SceneGlobals
{
    FCameraView CameraView;
    float Time;
    float DeltaTime;
} SceneUBO;

layout(set = 0, binding = 1) readonly buffer FModelData
{
    mat4 ModelMatrix[]; // Raw flat buffer of scatterd models (not sorted).
} ModelData;

layout(set = 0, binding = 2) readonly buffer FLightData
{
    uint                NumPointLights;
    bool                bHasDirectionalLight;
    FDirectionalLight   DirectionalLight;
    FPointLight         PointLights[];
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

bool HasDirectionalLight()
{
    return LightData.bHasDirectionalLight;
}

FDirectionalLight GetDirectionalLight()
{
    return LightData.DirectionalLight;
}

