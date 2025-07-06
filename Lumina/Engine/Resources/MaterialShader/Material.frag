#version 450 core
#pragma shader_stage(fragment)

#include "Includes/SceneGlobals.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 GPosition;
layout(location = 1) out vec3 GNormal;
layout(location = 2) out vec4 GAlbedoSpec;

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

void main()
{
    SMaterialInputs mat = GetMaterialInputs();

    GPosition = inFragPos;
    
    GNormal = normalize(mat.Normal);
    
    GAlbedoSpec.rgb = mat.Diffuse.rgb;
    
    GAlbedoSpec.a = mat.Specular;
}
