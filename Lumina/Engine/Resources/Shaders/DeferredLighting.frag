#version 450

#pragma shader_stage(fragment)

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uPosition;
layout(set = 0, binding = 1) uniform sampler2D uNormal;
layout(set = 0, binding = 2) uniform sampler2D uAlbedoSpec;

void main() 
{
    vec3 Position = texture(uPosition, vUV).rgb;
    vec3 Normal = texture(uNormal, vUV).rgb;
    vec3 Albedo = texture(uAlbedoSpec, vUV).rgb;
    float Specular = texture(uAlbedoSpec, vUV).a;

    outColor = vec4(Albedo, 1.0);
}
