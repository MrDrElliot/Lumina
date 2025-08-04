#version 460 core
#extension GL_ARB_shader_draw_parameters : enable

#pragma shader_stage(vertex)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

// Input attributes
layout(location = 0) in vec4 inPosition;

precise invariant gl_Position;

void main()
{
	mat4 Model = GetModelMatrix(gl_InstanceIndex);
	vec4 WorldPos = Model * vec4(inPosition.xyz, 1.0f);

	mat4 View = GetCameraView();
	mat4 Projection = GetCameraProjection();

	gl_Position = Projection * View * WorldPos;
}
