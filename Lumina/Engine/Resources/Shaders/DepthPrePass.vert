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
	mat4 ModelMatrix = GetModelMatrix(gl_InstanceIndex);
	mat4 View = GetCameraView();
	
	vec4 ViewPosition = View * ModelMatrix * vec4(inPosition.xyz, 1.0);

	gl_Position = GetCameraProjection() * ViewPosition;
}
