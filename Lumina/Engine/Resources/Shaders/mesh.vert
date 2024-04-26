#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 vertColor;
layout(location = 1) out vec2 outUV;

layout( push_constant ) uniform constants
{
    int data;
} PushC;

// Define the uniform buffer for the ViewProjection matrix
layout(binding = 0) uniform CameraData
{
    mat4 ViewProjection;
} camData;


// Transform buffer
layout(binding = 1) buffer ModelData
{
    mat4 Transform[];
} modelData;

layout(binding = 2) uniform sampler2D displayTexture;

// Lighting parameters
vec3 lightDirection = normalize(vec3(1.0, -1.0, -1.0)); // Top-right light
vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
vec3 ambientColor = vec3(0.2, 0.2, 0.2); // Low ambient lighting

vec4 pseudoRandomColor(int seed)
{
    int n = seed * 65536;
    n = (n >> 13) ^ n;
    int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    float r = float((nn & 0xFF000000) >> 24) / 255.0;
    float g = float((nn & 0x00FF0000) >> 16) / 255.0;
    float b = float((nn & 0x0000FF00) >> 8) / 255.0;
    return vec4(r, g, b, 1.0);
}


void main()
{
    
    //gl_Position = camData.ViewProjection * modelData.Transform[gl_InstanceIndex] * vec4(inPosition, 1.0);
    vec4 worldPosition = modelData.Transform[gl_InstanceIndex] * vec4(inPosition, 1.0);
    gl_Position = camData.ViewProjection * worldPosition;
    

    vertColor = texture(displayTexture, inUV); //pseudoRandomColor(gl_InstanceIndex); //inColor;
    outUV = inUV;
}