#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants {
    vec3 translation;
    vec3 color;
} pc;

layout(location = 0) out vec3 fragColor;

void main() {
    vec2 positions[3] = vec2[](
        vec2(0.0, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, 0.5)
    );

    // Apply translation to the position
    vec2 pos = positions[gl_VertexIndex] + pc.translation.xy;

    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = pc.color;  // Use the push constant color
}
