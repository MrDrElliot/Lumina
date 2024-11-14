#version 450

// Output UV coordinates to fragment shader
layout(location = 0) out vec2 fragUV;

void main()
{
    // Use the vertex index to define the corners of a fullscreen triangle
    vec2 positions[3] = vec2[]
    (
        vec2(-1.0, -1.0),  // Bottom-left
        vec2(3.0, -1.0),   // Bottom-right (goes beyond NDC to cover full screen with a triangle)
        vec2(-1.0, 3.0)    // Top-left (goes beyond NDC)
    );

    // Set position and compute UVs
    vec2 pos = positions[gl_VertexIndex];
    gl_Position = vec4(pos, 0.0, 1.0);

    // UVs in [0,1] range
    fragUV = pos * 0.5 + 0.5;
}
