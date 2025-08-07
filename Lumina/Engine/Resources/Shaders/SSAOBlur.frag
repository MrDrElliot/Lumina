#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

layout(set = 1, binding = 0) uniform sampler2D uSSAOInput;

layout(location = 0) in vec2 inUV;

layout(location = 0) out float outFragColor;


void main()
{
    vec2 TexelSize = 1.0 / vec2(textureSize(uSSAOInput, 0));
    float Result = 0.0f;
    for(int x = -2; x < 2; ++x)
    {
        for(int y = -2; y < 2; ++y)
        {
            vec2 Offset = vec2(float(x), float(y)) * TexelSize;
            Offset.y = -Offset.y;

            Result += texture(uSSAOInput, inUV + Offset).r;
        }
    }
    
    outFragColor = Result / (4.0 * 4.0);
}
