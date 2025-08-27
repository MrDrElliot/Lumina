#version 450
#pragma shader_stage(fragment)

#include "Includes/Common.glsl"
#include "Includes/SceneGlobals.glsl"

layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec2 vUV;

layout(set = 1, binding = 0) uniform EnvironmentUBO
{
    vec3 SunDirection;
} UBO;

// Improved sun color calculation with proper day/night cycle
vec3 calculateSunColor() {
    float sunHeight = UBO.SunDirection.y;

    // Define color phases
    vec3 nightColor = vec3(0.1, 0.15, 0.3) * 0.8;       // Cooler night
    vec3 twilightColor = vec3(1.0, 0.4, 0.15) * 12.0;   // Warm twilight
    vec3 sunriseColor = vec3(1.0, 0.7, 0.3) * 18.0;     // Golden sunrise
    vec3 dayColor = vec3(1.0, 0.95, 0.85) * 20.0;       // Bright daylight

    vec3 sunColor;
    if (sunHeight < -0.3) 
    {
        sunColor = nightColor;
    } 
    else if (sunHeight < -0.05) 
    {
        float t = smoothstep(-0.3, -0.05, sunHeight);
        sunColor = mix(nightColor, twilightColor, t);
    } 
    else if (sunHeight < 0.05)
    {
        float t = smoothstep(-0.05, 0.05, sunHeight);
        sunColor = mix(twilightColor, sunriseColor, t);
    } 
    else if (sunHeight < 0.4) 
    {
        float t = smoothstep(0.05, 0.4, sunHeight);
        sunColor = mix(sunriseColor, dayColor, t);
    } 
    else 
    {
        sunColor = dayColor;
    }

    return sunColor;
}

vec3 uSunColor = calculateSunColor();
const float uSunSize = radians(0.53);
const vec3 uSkyZenith = vec3(0.05, 0.1, 0.4);
const vec3 uSkyHorizon = vec3(0.6, 0.8, 1.0);
const vec3 uGroundColor = vec3(0.4, 0.25, 0.1);

// Improved scattering coefficients
const vec3 rayleighCoeff = vec3(5.8e-6, 13.5e-6, 33.1e-6);
const float mieCoeff = 2.1e-5;
const float mieG = 0.76;

// Atmosphere parameters
const float atmosphereRadius = 6420e2;
const float planetRadius = 6360e2;
const float rayleighHeight = 8e2;
const float mieHeight = 1.2e2;

// Improved view ray calculation
vec3 getViewRay(vec2 uv) 
{
    uv.y = 1.0 - uv.y;
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 rayClip = vec4(ndc, 1.0, 1.0);
    vec4 rayView = GetInverseCameraProjection() * rayClip;
    rayView.xyz /= rayView.w;

    vec3 rayWorld = (GetInverseCameraView() * vec4(rayView.xyz, 0.0)).xyz;
    return normalize(rayWorld);
}

// Optimized phase functions
float rayleighPhase(float cosTheta)
{
    const float factor = 3.0 / (16.0 * 3.14159);
    return factor * (1.0 + cosTheta * cosTheta);
}

float miePhase(float cosTheta, float g)
{
    const float factor = 1.0 / (4.0 * 3.14159);
    float g2 = g * g;
    float denom = 1.0 + g2 - 2.0 * g * cosTheta;
    return factor * (1.0 - g2) / (denom * sqrt(denom));
}

// Ray-sphere intersection
vec2 raySphereIntersect(vec3 rayOrigin, vec3 rayDir, float radius)
{
    float b = dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - radius * radius;
    float discriminant = b * b - c;

    if (discriminant < 0.0) {
        return vec2(-1.0);
    }

    float sqrt_disc = sqrt(discriminant);
    return vec2(-b - sqrt_disc, -b + sqrt_disc);
}

// Optimized optical depth calculation
vec2 computeOpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
    const int steps = 8; // Reduced for better performance
    float stepSize = rayLength / float(steps);

    vec2 opticalDepth = vec2(0.0);

    for (int i = 0; i < steps; i++) {
        vec3 samplePoint = rayOrigin + rayDir * (float(i) + 0.5) * stepSize;
        float height = length(samplePoint) - planetRadius;

        if (height < 0.0) break;

        // Precompute exponentials
        float rayleighDensity = exp(-height / rayleighHeight);
        float mieDensity = exp(-height / mieHeight);

        opticalDepth += vec2(rayleighDensity, mieDensity) * stepSize;
    }

    return opticalDepth;
}

// Enhanced atmospheric scattering with better performance
vec3 computeAtmosphericScattering(vec3 rayOrigin, vec3 rayDir)
{
    vec2 atmosphereIntersect = raySphereIntersect(rayOrigin, rayDir, atmosphereRadius);

    if (atmosphereIntersect.y < 0.0)
    {
        return vec3(0.0);
    }

    float rayStart = max(atmosphereIntersect.x, 0.0);
    float rayEnd = atmosphereIntersect.y;

    // Check ground intersection
    vec2 groundIntersect = raySphereIntersect(rayOrigin, rayDir, planetRadius);
    if (groundIntersect.x > 0.0) 
    {
        rayEnd = min(rayEnd, groundIntersect.x);
    }

    float rayLength = rayEnd - rayStart;
    if (rayLength <= 0.0) return vec3(0.0);

    const int primarySteps = 12;
    float primaryStepSize = rayLength / float(primarySteps);

    vec3 totalRayleigh = vec3(0.0);
    vec3 totalMie = vec3(0.0);
    vec2 primaryOpticalDepth = vec2(0.0);

    for (int i = 0; i < primarySteps; i++)
    {
        vec3 primarySamplePoint = rayOrigin + rayDir * (rayStart + (float(i) + 0.5) * primaryStepSize);
        float primaryHeight = length(primarySamplePoint) - planetRadius;

        if (primaryHeight < 0.0) break;

        float primaryRayleighDensity = exp(-primaryHeight / rayleighHeight);
        float primaryMieDensity = exp(-primaryHeight / mieHeight);

        primaryOpticalDepth += vec2(primaryRayleighDensity, primaryMieDensity) * primaryStepSize;

        vec2 lightIntersect = raySphereIntersect(primarySamplePoint, UBO.SunDirection, atmosphereRadius);
        float lightRayLength = lightIntersect.y;

        vec2 lightOpticalDepth = computeOpticalDepth(primarySamplePoint, UBO.SunDirection, lightRayLength);
        vec2 totalOpticalDepth = primaryOpticalDepth + lightOpticalDepth;

        vec3 rayleighTransmittance = exp(-rayleighCoeff * totalOpticalDepth.x);
        vec3 mieTransmittance = exp(-vec3(mieCoeff) * totalOpticalDepth.y * 1.1);

        totalRayleigh += primaryRayleighDensity * rayleighTransmittance * primaryStepSize;
        totalMie += primaryMieDensity * mieTransmittance * primaryStepSize;
    }

    float cosTheta = dot(rayDir, UBO.SunDirection);

    float rayleighPhaseValue = rayleighPhase(cosTheta);
    float miePhaseValue = miePhase(cosTheta, mieG);

    vec3 rayleighColor = rayleighCoeff * rayleighPhaseValue * totalRayleigh;
    vec3 mieColor = vec3(mieCoeff * miePhaseValue) * totalMie;

    return uSunColor * (rayleighColor + mieColor);
}

vec3 renderSun(vec3 rayDir)
{
    float sunDot = dot(rayDir, UBO.SunDirection);

    // Main sun disc
    float sunDisc = smoothstep(cos(uSunSize * 0.5), cos(uSunSize * 0.25), sunDot);

    // Sun corona/glow effect
    float sunGlow = smoothstep(cos(uSunSize * 1.8), cos(uSunSize * 1.1), sunDot);
    sunGlow = pow(sunGlow, 3.0);

    float sunFade = smoothstep(0, 0.1, UBO.SunDirection.y);

    vec3 sunColor = uSunColor * (sunDisc * 0.8 + sunGlow * 0.3) * sunFade;

    return sunColor;
}

void main() {
    vec3 rayDir = getViewRay(vUV);

    vec3 cameraPos = vec3(0.0, planetRadius + 50.0, 0.0);

    vec3 color = vec3(0.0);

    color += computeAtmosphericScattering(cameraPos, rayDir);

    color += renderSun(rayDir);

    vec2 groundIntersect = raySphereIntersect(cameraPos, rayDir, planetRadius);
    if (groundIntersect.x > 0.0)
    {
        float groundDot = max(dot(UBO.SunDirection, vec3(0.0, 1.0, 0.0)), 0.0);
        vec3 groundColor = uGroundColor * (0.3 + 0.7 * groundDot);

        float distance = groundIntersect.x;
        float atmosphericFade = exp(-distance * 1e-5);

        float horizonGlow = smoothstep(-0.92, 0.1, rayDir.y);

        color = mix(groundColor * atmosphericFade, color, horizonGlow);
    }

    float avgLuminance = dot(color, vec3(0.299, 0.587, 0.114));
    float exposure = 1.0 / (1.0 + avgLuminance * 0.1);
    exposure = mix(1.2, 2.5, smoothstep(-0.2, 0.3, UBO.SunDirection.y));

    color = 1.0 - exp(-color * exposure);
    color = pow(color, vec3(1.0 / 2.2));

    color = mix(color, color * color * (3.0 - 2.0 * color), 0.1);

    fragColor = vec4(color, 1.0);
}