

vec3 CubeVertices[36] = vec3[](
    // Front
    vec3(-1, -1,  1), vec3(1, -1,  1), vec3(1, 1,  1),
    vec3(-1, -1,  1), vec3(1, 1,  1), vec3(-1, 1,  1),
    // Back
    vec3(-1, -1, -1), vec3(1, 1, -1), vec3(1, -1, -1),
    vec3(-1, -1, -1), vec3(-1, 1, -1), vec3(1, 1, -1),
    // Left
    vec3(-1, -1, -1), vec3(-1, -1,  1), vec3(-1, 1,  1),
    vec3(-1, -1, -1), vec3(-1, 1,  1), vec3(-1, 1, -1),
    // Right
    vec3(1, -1, -1), vec3(1, 1,  1), vec3(1, -1,  1),
    vec3(1, -1, -1), vec3(1, 1, -1), vec3(1, 1,  1),
    // Top
    vec3(-1, 1, -1), vec3(-1, 1,  1), vec3(1, 1,  1),
    vec3(-1, 1, -1), vec3(1, 1,  1), vec3(1, 1, -1),
    // Bottom
    vec3(-1, -1, -1), vec3(1, -1,  1), vec3(-1, -1,  1),
    vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1, -1,  1)
);