#version 450 core

#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

// Uniform data
layout(set = 0, binding = 0) uniform UBO
{
    float time;
};

// Output color
layout(location = 0) out vec4 outFragColor;

// Function to generate random values based on position and time (hash function)
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Function to simulate star twinkling
float starTwinkle(float time, vec2 position) {
    // Use sine to create twinkling effect based on time and position
    return sin(time + position.x * 10.0) * 0.5 + 0.5;  // Twinkle effect, 0.0 to 1.0
}

void main()
{
    // Normalize fragment coordinates from 0 to 1
    vec2 uv = gl_FragCoord.xy / vec2(1920.0, 1080.0);  // Assume resolution is 1920x1080 for simplicity

    // Create a grid of "stars" by using random positions
    vec2 starPos = fract(uv * 100.0);  // Adjust the multiplier to control star density
    float star = random(starPos);  // Random value for each star

    // Apply star density by limiting the maximum value of stars
    float starIntensity = smoothstep(0.8, 1.0, star);  // Threshold to create visible stars

    // Create a "star field" effect with random star positions and intensity
    float dist = length(uv - vec2(0.5, 0.5));  // Distance from the center of the screen
    starIntensity *= 1.0 - dist;  // Stars get dimmer as they move away from the center

    // Twinkle effect based on position and time
    float twinkleEffect = starTwinkle(time, starPos);

    // Final star color (white for the stars)
    vec3 starColor = vec3(1.0, 1.0, 1.0) * starIntensity * twinkleEffect;

    // Apply a dark background and draw stars in the foreground
    outFragColor = vec4(starColor, 1.0);
}
