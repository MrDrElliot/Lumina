#version 450

#pragma shader_stage(vertex)

#include "Includes/Primitives.glsl"

layout(push_constant) uniform PushConsts {
	layout(offset = 0)  mat4 mvp;
	layout(offset = 64) float deltaPhi;
	layout(offset = 68) float deltaTheta;
} pushConsts;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
	outUVW = CubeVertices[gl_VertexIndex];
	gl_Position = pushConsts.mvp * vec4(outUVW.xyz, 1.0);
}
