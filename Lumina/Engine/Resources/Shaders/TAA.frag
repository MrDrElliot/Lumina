#version 450

// Screen-space UV coordinates
layout(location = 0) in vec2 fragUV;

// Input color texture (from the current frame)
layout(set = 0, binding = 0) uniform sampler2D currentFrameColor;

// History buffer texture (from the previous frame)
layout(set = 0, binding = 1) uniform sampler2D historyColor;

// Motion vector texture (optional, for reprojecting previous frame pixels)
layout(set = 0, binding = 2) uniform sampler2D motionVectorTexture;

// Output color
layout(location = 0) out vec4 outColor;

// Constants for TAA blending
const float ALPHA = 0.1;  // Blending factor, adjust for preferred smoothness
const float CLAMP_THRESHOLD = 0.1;  // Threshold to avoid ghosting or artifacts


void main() 
{
    // Sample current frame color
    vec4 currentColor = texture(currentFrameColor, fragUV);

    // Sample the motion vector at this fragment (motion vector in screen-space)
    vec2 motionVector = texture(motionVectorTexture, fragUV).xy;

    // Calculate the history buffer UV based on the motion vector
    vec2 historyUV = fragUV - motionVector;

    // Sample the color from the history buffer at the reprojected UV
    vec4 historyColor = texture(historyColor, historyUV);

    // Perform blending of current color and history color with clamping to reduce ghosting
    vec4 blendedColor = mix(currentColor, historyColor, ALPHA);

    // Clamp based on a threshold to minimize ghosting artifacts
    vec3 colorDifference = abs(currentColor.rgb - historyColor.rgb);
    if (colorDifference.r > CLAMP_THRESHOLD || colorDifference.g > CLAMP_THRESHOLD || colorDifference.b > CLAMP_THRESHOLD)
    {
        blendedColor.rgb = currentColor.rgb;
    }

    // Output the final blended color
    outColor = blendedColor;
}
