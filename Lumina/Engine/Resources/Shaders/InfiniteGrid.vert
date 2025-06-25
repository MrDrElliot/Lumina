#version 450 core
#pragma shader_stage(vertex)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"



// Define outputs to the fragment shader
layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out mat4 fragView;   // Passing view matrix
layout(location = 7) out vec3 CameraPos;
layout(location = 8) out mat4 fragProj;   // Passing projection matrix

// Grid position in clip space
vec3 gridPlane[6] = vec3[]
(
    vec3( 1,  1, 0),
    vec3(-1, -1, 0),
    vec3(-1,  1, 0),
    vec3(-1, -1, 0),
    vec3( 1,  1, 0),
    vec3( 1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    vec3 p = gridPlane[gl_VertexIndex].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, GetCameraView(), GetCameraProjection()).xyz; // Unproject on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, GetCameraView(), GetCameraProjection()).xyz;  // Unproject on the far plane

    fragView = GetCameraView();
    fragProj = GetCameraProjection();

    CameraPos = GetCameraPosition().xyz;

    gl_Position = vec4(p, 1.0);
}