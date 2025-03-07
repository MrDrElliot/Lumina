#version 450 core

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"

layout(location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outColor;

void main()
{
    //const array of colors for the triangle
    const vec3 colors[3] = vec3[3](
    vec3(1.0f, 0.0f, 0.0f), //red
    vec3(0.0f, 1.0f, 0.0f), //green
    vec3(00.f, 0.0f, 1.0f)  //blue
    );

    //output the position of each vertex
    gl_Position = vec4(inPosition, 1.0f);
    outColor = colors[gl_VertexIndex];
}
