#version 450

#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragNormal;                    // Original normal (from vertex)
layout(location = 1) in vec3 fragWorldNormal;              // Input world normal for fragment shader
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

// All textues to be rendered, accessed from the texture ID.
layout(set = 0, binding = 1) uniform sampler2D Textures[MAX_TEXTURES];

// Material instance properties.
layout(push_constant) uniform MaterialProperties
{
    layout(offset = 16) vec4 baseColor;
    float roughness;
    float metallic;
    float emissiveIntensity;
} material;

// ID to correlate to the Textures[MAX_TEXTURES] sampler array.
struct MaterialTexturesData
{
    int AlbedoID;
    int NormalID;
    int RoughnessID;
    int EmissiveID;
    int AOID;
};

// Generic Point light
struct Light
{
    vec4 LightPosition;
    vec4 LightColor;
};

// All point lights in the scene (non-directional).
struct SceneLightData
{
    int NumLights;
    Light Lights[MAX_LIGHTS];
};

// Total uniform of all texture IDs to be used.
layout(set = 1, binding = 3) uniform MaterialTextureBuffer
{
    MaterialTexturesData MaterialTextures[MAX_MATERIALS];
} MaterialTextureIDs;

// Buffer containing all the lights in the scene.
layout(set = 1, binding = 1) uniform SceneLightBuffer
{
    SceneLightData LightData;
} SceneLight;

// Try to get a texture, or fallback to a default value if NO_TEXTURE is specified.
vec4 sampleTextureOrDefault(int textureIndex, vec2 texCoord, vec4 defaultValue)
{
    if (textureIndex == NO_TEXTURE)
    {
        return defaultValue;
    }

    return texture(Textures[textureIndex], texCoord);
}

// Helper to sample all textures.
void sampleTextures(out vec4 albedo, out vec3 normal, out float roughness, out float metallic, out vec3 emissive, out float ao)
{
    albedo = sampleTextureOrDefault(MaterialTextureIDs.MaterialTextures[inMaterialInstanceIndex].AlbedoID, fragTexCoord, vec4(1.0, 1.0, 1.0, 1.0));

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

void main()
{
    vec4 Albedo;
    vec3 Normal, Emissive;
    float Roughness, AmbientOcclusion, Metallic;
    sampleTextures(Albedo, Normal, Roughness, Metallic, Emissive, AmbientOcclusion);

    vec3 AccumulatedLight = vec3(0.0); // Initialize the final light contribution

    // Iterate through all lights in the scene
    for (int i = 0; i < SceneLight.LightData.NumLights; ++i)
    {
        vec3 LightPos = SceneLight.LightData.Lights[i].LightPosition.xyz;
        vec3 LightColor = SceneLight.LightData.Lights[i].LightColor.xyz;
        float LightIntensity = SceneLight.LightData.Lights[i].LightColor.w;

        vec3 DirectionToLight = LightPos - fragWorldPosition;
        float distanceSquared = dot(DirectionToLight, DirectionToLight);
        float Attenuation = 1.0 / distanceSquared;

        vec3 FinalLightColor = LightColor * LightIntensity * Attenuation;

        // Calculate diffuse lighting using Albedo
        vec3 DiffuseLight = FinalLightColor * max(dot(normalize(fragWorldNormal), normalize(DirectionToLight)), 0.0) * Albedo.rgb;

        // Accumulate light contributions
        AccumulatedLight += DiffuseLight;
    }

    // Add emissive lighting
    vec3 EmissiveLight = Emissive;

    // Add ambient light (modulated by Albedo and AmbientOcclusion)
    vec3 AmbientLight = vec3(0.1) * Albedo.rgb * AmbientOcclusion;

    // Combine all lighting contributions
    vec3 FinalColor = AccumulatedLight + AmbientLight + EmissiveLight;

    outColor = vec4(FinalColor, 1.0); // Output the final color
}



