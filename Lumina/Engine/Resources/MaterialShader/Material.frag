#version 460 core

#pragma shader_stage(fragment)

#include "Includes/SceneGlobals.glsl"

#define MAX_SCALARS 24
#define MAX_VECTORS 24

layout(early_fragment_tests) in;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec2 inUV;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out vec4 GPosition;
layout(location = 1) out vec4 GNormal;
layout(location = 2) out vec4 GMaterial;
layout(location = 3) out vec4 GAlbedoSpec;

layout(set = 1, binding = 0) uniform FMaterialUniforms
{
    vec4 Scalars[MAX_SCALARS / 4];
    vec4 Vectors[MAX_VECTORS];

} MaterialUniforms;

float GetMaterialScalar(uint Index)
{
    uint v = Index / 4;
    uint c = Index % 4;
    return MaterialUniforms.Scalars[v][c];
}

vec4 GetMaterialVec4(uint Index)
{
    return MaterialUniforms.Vectors[Index];
}

vec3 GetWorldNormal(vec3 FragNormal, vec2 UV, vec3 FragPos, vec3 TangentSpaceNormal)
{
    vec3 N = normalize(FragNormal);

    vec3 Q1 = dFdx(FragPos);
    vec3 Q2 = dFdy(FragPos);
    vec2 st1 = dFdx(UV);
    vec2 st2 = dFdy(UV);

    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * TangentSpaceNormal);
}

struct SMaterialInputs
{
    vec3    Diffuse;
    float   Metallic;
    float   Roughness;
    float   Specular;
    vec3    Emissive;
    float   AmbientOcclusion;
    vec3    Normal;
    float   Opacity;
};

$MATERIAL_INPUTS

void main()
{
    SMaterialInputs Material = GetMaterialInputs();

    GPosition = vec4(inFragPos, LinearDepth(gl_FragCoord.w));

    vec3 WorldNormal = GetWorldNormal(inNormal, inUV, inFragPos, Material.Normal);
    GNormal = vec4(WorldNormal * 0.5 + 0.5, 1.0);

    GMaterial.r = Material.AmbientOcclusion;
    GMaterial.g = Material.Roughness;
    GMaterial.b = Material.Metallic;
    GMaterial.a = 1.0;

    GAlbedoSpec.rgb = Material.Diffuse.rgb;
    GAlbedoSpec.a = Material.Specular;
}
