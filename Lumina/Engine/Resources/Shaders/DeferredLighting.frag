#version 450

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D uPosition;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uMaterial;
layout(set = 1, binding = 3) uniform sampler2D uAlbedoSpec;


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
    vec3 Position = texture(uPosition, vUV).rgb;
    vec3 Albedo = texture(uAlbedoSpec, vUV).rgb;
    vec3 Normal = texture(uNormal, vUV).rgb;
    float AO = texture(uMaterial, vUV).r;
    float Roughness = texture(uMaterial, vUV).g;
    float Metallic = texture(uMaterial, vUV).b;
    float Specular = texture(uAlbedoSpec, vUV).a;


    vec3 N = Normal;
    vec3 V = normalize(SceneUBO.CameraView.CameraPosition.xyz - Position);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo.rgb, Metallic);

    vec3 Lo = vec3(0.0);
    if (HasDirectionalLight())
    {
        FDirectionalLight Light = GetDirectionalLight();

        vec3 L = normalize(-Light.Direction.xyz);
        vec3 H = normalize(V + L);
        vec3 Radiance = Light.Color.rgb * Light.Color.a;

        float NDF = DistributionGGX(N, H, Roughness);
        float G = GeometrySmith(N, V, L, Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 Numerator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 Spec = Numerator / Denominator;
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * Albedo / PI + Spec) * Radiance * NdotL;
    }
    
    for(int i = 0; i < LightData.NumPointLights; ++i)
    {
        FPointLight Light = LightData.PointLights[i];
        
        vec3 L = normalize(Light.Position.xyz - Position);
        vec3 H = normalize(V + L);
        float Distance = length(Light.Position.xyz - Position);
        float Attenuation = 1.0 / (Distance * Distance);
        vec3 Radiance = Light.Color.rgb * Light.Color.a * Attenuation;
        
        float NDF = DistributionGGX(N, H, Roughness);
        float G = GeometrySmith(N, V, L, Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 Numerator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 Spec = Numerator / Denominator;
        vec3 kS = F;
        
        vec3 kD = vec3(1.0) - kS;
        
        kD *= 1.0 - Metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        
        Lo += (kD * Albedo / PI + Spec) * Radiance * NdotL;
    }
    
    vec3 Ambient = vec3(0.01) * Albedo * AO;
    vec3 Color = Ambient + Lo;
    
    Color = Color / (Color + vec3(1.0));
    
    Color = pow(Color, vec3(1.0/2.2));

    outColor = vec4(Color, 1.0);
}
