#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) out vec4 vertColor;

void main()
{
    gl_Position = vec4(inPosition, 1.0); 
    vertColor = vec4(1.0, 0.0, 0.0, 1.0);
}