#version 460 core
#pragma shader_stage(fragment)

#include "Includes/SceneGlobals.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 GPosition;
layout(location = 1) out vec4 GNormal;
layout(location = 2) out vec4 GMaterial;
layout(location = 3) out vec4 GAlbedoSpec;

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

$MATERIAL_INPUTS


vec3 getNormalFromMap(vec3 Normal)
{
    vec3 tangentNormal = Normal * 2.0 - 1.0;

    vec3 Q1  = dFdx(inFragPos);
    vec3 Q2  = dFdy(inFragPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N   = normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    SMaterialInputs mat = GetMaterialInputs();

    GPosition = vec4(inFragPos, 1.0);

    GNormal = vec4(getNormalFromMap(mat.Normal), 1.0);
    
    GMaterial.r = mat.AmbientOcclusion;
    GMaterial.g = mat.Roughness;
    GMaterial.b = mat.Metallic;
    GMaterial.a = 1.0;
    
    GAlbedoSpec.rgb = mat.Diffuse.rgb;
    
    GAlbedoSpec.a = mat.Specular;
}
