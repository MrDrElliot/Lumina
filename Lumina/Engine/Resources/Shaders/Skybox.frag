#version 450
#pragma shader_stage(fragment)

layout(set = 1, binding = 0) uniform samplerCube uCubeMap;

layout(location = 0) in vec3 fragNormal;
layout(location = 0) out vec4 fragColor;

void main()
{
    vec4 TextureSample = texture(uCubeMap, fragNormal);
    TextureSample.a = 1.0;
    fragColor = TextureSample;
}