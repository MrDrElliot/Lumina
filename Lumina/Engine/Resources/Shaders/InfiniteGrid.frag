#version 450 core

layout(location = 0) in float near;
layout(location = 1) in float far;
layout(location = 2) in vec3 nearPoint;
layout(location = 3) in vec3 farPoint;
layout(location = 4) in mat4 fragView;
layout(location = 8) in mat4 fragProj;
layout(location = 0) out vec4 outColor;

const float AXIS_THRESHOLD = 0.1;   // Threshold for axis line coloring
const float GRID_SCALE = 10.0;      // Grid scale factor
const float FADE_LIMIT = 0.50;      // Fading start threshold for linear depth

// Grid generation with axis drawing option
vec4 grid(vec3 fragPos3D, float scale, bool drawAxis)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    // Z-axis detection
    if (fragPos3D.x > -AXIS_THRESHOLD * minimumx && fragPos3D.x < AXIS_THRESHOLD * minimumx)
    color.z = 1.0;

    // X-axis detection
    if (fragPos3D.z > -AXIS_THRESHOLD * minimumz && fragPos3D.z < AXIS_THRESHOLD * minimumz)
    color.x = 1.0;

    return color;
}

// Compute depth in clip space
float computeDepth(vec3 pos)
{
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

// Compute linear depth
float computeLinearDepth(vec3 pos)
{
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - clip_space_depth * (far - near)) / far;
}

void main()
{
    // Interpolated position between near and far points
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    // Set the fragment depth in clip space
    gl_FragDepth = computeDepth(fragPos3D);

    // Linear depth calculation for fading effect
    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (FADE_LIMIT - linearDepth));

    // Apply grid and axis coloring
    outColor = (grid(fragPos3D, GRID_SCALE, true) + grid(fragPos3D, 1.0, true)) * float(t > 0);
    outColor.a *= fading;  // Apply fading based on depth
    
}
