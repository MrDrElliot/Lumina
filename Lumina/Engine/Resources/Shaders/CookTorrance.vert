#version 450

layout(location = 0) in vec3 inPosition;            // Vertex position in object space
layout(location = 1) in vec3 inNormal;              // Vertex normal in object space
layout(location = 2) in vec2 inTexCoord;            // Texture coordinates
layout(location = 3) in vec4 inColor;               // Vertex color (if any)

//////////////////////////////////////////////////////////////////////////////////////////////////////

layout(location = 0) out vec3 fragNormal;           // Output normal for fragment shader
layout(location = 1) out vec2 fragTexCoord;         // Output texture coordinates
layout(location = 2) out vec3 fragViewPosition;     // Output fragment position in view space
layout(location = 3) out vec3 fragWorldPosition;    // Output fragment position in world space
layout(location = 4) out uint outModelIndex;        // Index of the model being rendered.

layout(push_constant) uniform ModelIndex
{
    uint Index;
} modelIndex;

layout(set = 1, binding = 0) uniform CameraUniforms
{
    mat4 view;  // View matrix
    mat4 proj;  // Projection matrix
} CameraUBO;

layout(set = 1, binding = 2) buffer ModelUniforms
{
    mat4 model[]; // Model matrix
} ModelUBO;

void main()
{
    
    uint Index = modelIndex.Index;
    outModelIndex = Index;
    
    // Calculate world position of the fragment.
    fragWorldPosition = vec3(ModelUBO.model[Index] * vec4(inPosition, 1.0));

    // Transform position to view space
    fragViewPosition = vec3(CameraUBO.view * vec4(fragWorldPosition, 1.0));

    // Transform the normal from object space to view space
    // Assuming ubo.model and ubo.view include rotation and translation
    fragNormal = normalize(mat3(CameraUBO.view) * mat3(ModelUBO.model[Index]) * inNormal);

    fragTexCoord = inTexCoord;

    // Set the final vertex position for rendering
    gl_Position = CameraUBO.proj * vec4(fragViewPosition, 1.0);
}
