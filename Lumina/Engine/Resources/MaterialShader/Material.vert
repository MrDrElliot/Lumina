#version 450 core
#pragma shader_stage(vertex)

#include "Includes/SceneGlobals.glsl"

// Input attributes
layout(location = 0) in vec3 inPosition;   // Vertex position (in object space)
layout(location = 1) in vec3 inNormal;     // Normal vector (in object space)
layout(location = 2) in vec4 inColor;      // Color (optional)

// Outputs
layout(location = 0) out vec3 outFragColor; // Color to fragment shader
layout(location = 1) out vec3 outNormal;    // Normal to fragment shader
layout(location = 2) out vec3 outWorldPos;  // World position to fragment shader

// Material inputs structure
struct SMaterialInputs
{
    vec3 Diffuse;
    float Metallic;
    float Roughness;
    float Specular;
    vec3 Emissive;
    float AmbientOcclusion;
    vec3 Normal;
    float Opacity;
};

// This gets replaced during material compliation.
$VERTEX_REPLACEMENT

void main()
{
    mat4 model = GetModelMatrix();
    vec4 worldPos = model * vec4(inPosition, 1.0);

    vec3 worldNormal = normalize(mat3(model) * inNormal);

    mat4 view = GetCameraView();
    mat4 projection = GetCameraProjection();

    gl_Position = projection * view * worldPos;

    outFragColor = GetMaterialInputs().Diffuse;
    outNormal = worldNormal;
    outWorldPos = worldPos.xyz;
}
