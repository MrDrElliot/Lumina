#version 450
#pragma shader_stage(vertex)

#include "Includes/Primitives.glsl"
#include "Includes/SceneGlobals.glsl"


layout(location = 0) out vec3 fragNormal;

void main()
{
    vec3 Pos = CubeVertices[gl_VertexIndex];
    mat4 View = mat4(mat3(GetCameraView()));
    gl_Position = GetCameraProjection() * View * vec4(Pos, 1.0);
    
    gl_Position.z = 0.0; // This is to force the skybox to be drawn behind everything. Setting the depth to be 0.0
    fragNormal = Pos;
}