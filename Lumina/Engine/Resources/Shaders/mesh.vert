#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) out vec4 vertColor;

layout(push_constant) uniform PushConstant
{
    mat4 ViewProjection;
} pc;


void main()
{
    gl_Position = pc.ViewProjection * vec4(inPosition, 1.0); 
    vertColor = vec4(1.0, 0.0, 0.0, 1.0);
}