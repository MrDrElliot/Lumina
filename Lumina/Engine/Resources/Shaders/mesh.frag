#version 450
layout(location = 1) in vec4 vertColor;  // Receive color from vertex shader
layout(location = 0) out vec4 fragColor;  // Output to framebuffer

void main()
{
    fragColor = vertColor;  // Use the color passed from vertex shader
}