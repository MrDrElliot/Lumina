#version 450

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

// Input attributes
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec4 inNormal;
layout(location = 3) in vec2 inUV;


void main()
{
    for(uint i = 0; i < GetNumLights(); ++i)
    { 
        FLight Light = GetLightAt(i);
        if(Light.Type != LIGHT_TYPE_POINT)
        {
            mat4 Model = GetModelMatrix(gl_InstanceIndex);
        }   
    }
}
