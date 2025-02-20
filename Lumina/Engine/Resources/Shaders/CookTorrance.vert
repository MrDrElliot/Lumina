#version 450

#pragma shader_stage(vertex)

layout(location = 0) in vec3 inPosition;            // Vertex position in object space
layout(location = 1) in vec4 inColor;               // Vertex color (if any)
layout(location = 2) in vec3 inNormal;              // Vertex normal in object space
layout(location = 3) in vec2 inTexCoord;            // Texture coordinates

//////////////////////////////////////////////////////////////////////////////////////////////////////

layout(location = 0) out vec3 fragNormal;                      // Output normal for fragment shader
layout(location = 1) out vec3 fragWorldNormal;                 // Output world normal for fragment shader
layout(location = 2) out vec2 fragTexCoord;                    // Output texture coordinates
layout(location = 3) out vec3 fragViewPosition;                // Output fragment position in view space
layout(location = 4) out vec3 fragWorldPosition;               // Output fragment position in world space
layout(location = 5) out uint outModelIndex;                   // Index of the model being rendered.
layout(location = 6) out uint outMaterialInstanceIndex;        // Index of the model being rendered.
layout(location = 7) out vec3 outCameraPosition;               // Camera position in world space.

layout(push_constant) uniform ModelIndex
{
    uint ModelIndex;
    uint MaterialInstanceIndex;
} modelIndex;

layout(set = 1, binding = 0) readonly uniform CameraUniforms
{
    vec4 CameraPosition;    // Camera Position
    mat4 CameraView;        // View matrix
    mat4 CameraProjection;  // Projection matrix
} CameraUBO;

layout(set = 1, binding = 2) readonly buffer ModelUniforms
{
    mat4 model[]; // Model matrix
} ModelUBO;

void main()
{
    outCameraPosition = CameraUBO.CameraPosition.xyz;

    uint Index = modelIndex.ModelIndex;

    outModelIndex = Index;
    outMaterialInstanceIndex = modelIndex.MaterialInstanceIndex;

    // Calculate world position of the fragment.
    fragWorldPosition = vec3(ModelUBO.model[Index] * vec4(inPosition, 1.0));

    // Transform position to view space
    fragViewPosition = vec3(CameraUBO.CameraView * vec4(fragWorldPosition, 1.0));

    // Extract the 3x3 part of the model matrix (rotation and scaling only)
    mat3 modelMatrix3x3 = mat3(ModelUBO.model[Index]);

    // Compute the inverse of the 3x3 matrix
    mat3 inverseModelMatrix = inverse(modelMatrix3x3);

    // Transpose the inverse matrix to get the normal matrix
    mat3 normalMatrix = transpose(inverseModelMatrix);

    // Transform the normal to world space using the normal matrix
    fragWorldNormal = normalize(normalMatrix * inNormal);

    // Also output the fragment's normal in object space for debugging or visualization
    fragNormal = normalize(inNormal);

    fragTexCoord = inTexCoord;

    // Set the final vertex position for rendering
    gl_Position = CameraUBO.CameraProjection * vec4(fragViewPosition, 1.0);
}
