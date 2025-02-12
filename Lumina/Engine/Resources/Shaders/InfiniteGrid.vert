#version 450 core

layout(set = 0, binding = 0) uniform CameraUniforms
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
} CameraUBO;

// Define outputs to the fragment shader
layout(location = 0) out float fragNear;  // Passing near value
layout(location = 1) out float fragFar;   // Passing far value
layout(location = 2) out vec3 nearPoint;
layout(location = 3) out vec3 farPoint;
layout(location = 4) out mat4 fragView;   // Passing view matrix
layout(location = 8) out mat4 fragProj;   // Passing projection matrix

// Grid position in clip space
vec3 gridPlane[6] = vec3[]
(
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
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
    // Define near and far plane distances
    fragNear = 0.01f;
    fragFar = 1000.0f;

    vec3 p = gridPlane[gl_VertexIndex].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, CameraUBO.CameraView, CameraUBO.CameraProjection).xyz; // Unproject on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, CameraUBO.CameraView, CameraUBO.CameraProjection).xyz;  // Unproject on the far plane

    // Pass view and projection matrices
    fragView = CameraUBO.CameraView;
    fragProj = CameraUBO.CameraProjection;

    gl_Position = vec4(p, 1.0);
}
