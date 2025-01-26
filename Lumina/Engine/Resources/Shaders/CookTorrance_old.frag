#version 450

layout(location = 0) in vec3 fragNormal;                    // Original normal (from vertex)
layout(location = 1) out vec3 fragWorldNormal;                 // Output world normal for fragment shader
layout(location = 2) in vec2 fragTexCoord;                  // Texture coordinates
layout(location = 3) in vec3 fragViewPosition;              // Fragment position in view space
layout(location = 4) in vec3 fragWorldPosition;             // Fragment position in world space
layout(location = 5) flat in uint inModelIndex;             // Index of the model being rendered.
layout(location = 6) flat in uint inMaterialInstanceIndex;
layout(location = 7) in vec3 CameraPosition;                // Camera Position Passed from vertex.

layout(location = 0) out vec4 outColor;                     // Final output color

#define PI              3.14159
#define MAX_LIGHTS      102
#define MAX_TEXTURES    1024
#define MAX_MATERIALS   1024
#define NO_TEXTURE      -1

// Albedo texture sampler
layout(set = 0, binding = 1) uniform sampler2D Textures[MAX_TEXTURES];

// Material properties block
layout(push_constant) uniform MaterialProperties
{
    layout(offset = 16) vec4 baseColor;
    float roughness;
    float metallic;
    float emissiveIntensity;
} material;

struct MaterialTexturesData
{
    int AlbedoID;
    int NormalID;
    int RoughnessID;
    int EmissiveID;
    int AOID;
};

struct Light
{
    vec4 LightPosition;
    vec4 LightColor;
};

struct SceneLightData
{
    int NumLights;
    Light Lights[MAX_LIGHTS];
};

layout(set = 1, binding = 3) uniform MaterialTextureBuffer
{
    MaterialTexturesData MaterialTextures[MAX_MATERIALS];
} MaterialTextureIDs;

layout(set = 1, binding = 1) uniform SceneLightBuffer
{
    SceneLightData LightData;
} SceneLightParams;

// Step function (Heaviside function)
float stepf(float x)
{
    return x > 0.0 ? 1.0 : 0.0;
}

// Fresnel-Schlick approximation for reflectance at normal incidence
float Fresnel_Schlick(float cosTheta, float F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX Microfacet Distribution with Heaviside step function
float GGX_D(vec3 halfwayDir, vec3 normal, float roughness)
{
    float alpha = roughness * roughness;
    float NdotH = max(dot(normal, halfwayDir), 0.0);  // dot(normal, halfwayDir)
    float HdotN = max(dot(halfwayDir, normal), 0.0);  // dot(halfwayDir, normal)

    // Use the Heaviside step function to mask the distribution
    float mask = stepf(HdotN);  // Mask out any negative values

    float alpha2 = alpha * alpha;
    float denominator = (NdotH * NdotH) * (alpha2 - 1.0) + 1.0;
    return mask * (alpha2 / (PI * denominator * denominator));
}

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return nom / denom;
}

// Smith's Geometry Attenuation (G) with Heaviside step function
float Smith_G(float NdotL, float NdotV, float roughness)
{
    float alpha = roughness * roughness;
    float G1_L = 2.0 * NdotL / (NdotL + sqrt(1.0 + alpha * alpha * (1.0 - NdotL * NdotL)));
    float G1_V = 2.0 * NdotV / (NdotV + sqrt(1.0 + alpha * alpha * (1.0 - NdotV * NdotV)));
    return G1_L * G1_V;
}

// Cook-Torrance BRDF with Schlick's Fresnel and GGX distribution
vec3 CookTorranceBRDF(vec3 normal, vec3 lightDir, vec3 viewDir, float roughness, float metallic, vec3 albedo)
{
    // Halfway vector
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Dot products
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float HdotV = max(dot(halfwayDir, viewDir), 0.0);

    // Fresnel-Schlick approximation for reflectance at normal incidence
    float F0 = mix(0.04, dot(albedo, vec3(0.3, 0.59, 0.11)), metallic);
    float F = Fresnel_Schlick(HdotV, F0);

    // GGX Microfacet Distribution (D)
    float D = DistributionGGX(halfwayDir, normal, roughness);

    // Smith's Geometry Attenuation (G)
    float G = Smith_G(NdotL, NdotV, roughness);

    // Specular intensity using Cook-Torrance model
    float specularIntensity = D * F * G;

    // Diffuse term (Lambertian approximation)
    vec3 diffuse = (1.0 - F) * (1.0 - metallic) * albedo / PI * (1.0 - metallic);

    // Return the combined diffuse and specular contributions
    return (diffuse + specularIntensity * vec3(1.0)) * NdotL;
}

vec3 ReinhardToneMapping(vec3 color, float exposure)
{
    color *= exposure;
    color = color / (color + vec3(1.0));
    return pow(color, vec3(1.0 / 2.2));
}

// Helper function to sample textures with a fallback value
vec4 sampleTextureOrDefault(int textureIndex, vec2 texCoord, vec4 defaultValue)
{
    if (textureIndex == NO_TEXTURE)
    {
        return defaultValue;
    }

    return texture(Textures[textureIndex], texCoord);
}

// Sample textures and gather material properties
void sampleTextures(out vec3 normal, out float roughness, out float metallic, out vec3 emissive, out float ao)
{
    vec4 normalSample = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].NormalID, fragTexCoord, vec4(1.0, 1.0, 1.0, 1.0));
    normal = normalize(normalSample.rgb * 2.0 - 1.0);

    vec4 roughnessSample = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].RoughnessID, fragTexCoord, vec4(0.5));
    roughness = roughnessSample.g;

    vec4 metallicSample = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].RoughnessID, fragTexCoord, vec4(0.5));
    metallic = metallicSample.b;

    vec4 emissiveSample = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].EmissiveID, fragTexCoord, vec4(1.0));
    emissive = emissiveSample.rgb;

    vec4 aoSample = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].AOID, fragTexCoord, vec4(0.0));
    ao = aoSample.r;
}

void calculateLighting(out vec3 finalColor)
{
    finalColor = vec3(0.0); // Initialize the final color

    // Iterate over all lights and accumulate the lighting contributions
    for (int i = 0; i < SceneLightParams.LightData.NumLights; ++i)
    {
        vec3 lightPos = SceneLightParams.LightData.Lights[i].LightPosition.rgb;
        vec3 lightDir = normalize(lightPos - fragWorldPosition);
        float distance = length(lightPos - fragWorldPosition);

        // Attenuation factors
        float constantFactor = 1.0;
        float linearFactor = 0.09;
        float quadraticFactor = 0.032;

        float attenuation = 1.0 / (constantFactor + linearFactor * distance + quadraticFactor * (distance * distance));

        vec3 viewDir = normalize(CameraPosition.rgb - fragWorldPosition);

        vec3 normal, emissive;
        float roughness, metallic, ao;
        sampleTextures(normal, roughness, metallic, emissive, ao);

        int AlbedoTextIndex = MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].AlbedoID;
        vec4 albedo = sampleTextureOrDefault(AlbedoTextIndex, fragTexCoord, vec4(1.0, 1.0, 1.0, 1.0));

        vec3 lightColor = SceneLightParams.LightData.Lights[i].LightColor.rgb;
        float lightIntensity = SceneLightParams.LightData.Lights[i].LightColor.a;

        vec3 color = CookTorranceBRDF(normal, lightDir, viewDir, roughness, metallic, albedo.rgb);

        finalColor += color * lightColor * lightIntensity * attenuation;

        // Apply ambient occlusion
        finalColor *= ao;

        // Add emissive light to the final color
        finalColor += emissive * material.emissiveIntensity;
    }

    // Apply tone mapping after all lights have been accumulated
    finalColor = ReinhardToneMapping(finalColor, 1.0);
}



void main()
{
    vec3 emissive, normal;
    float roughness, metallic, ao;
    sampleTextures(normal, roughness, metallic, emissive, ao);
    
    vec3 finalColor = vec3(0.0);
    calculateLighting(finalColor);

    int AlbedoTextIndex = MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].AlbedoID;
    vec4 albedo = sampleTextureOrDefault(AlbedoTextIndex, fragTexCoord, vec4(1.0, 1.0, 1.0, 1.0));
    
    outColor = vec4(finalColor, albedo.a);
}
