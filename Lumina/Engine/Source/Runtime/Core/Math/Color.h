#pragma once
#include <algorithm>
#include <string>
#include <cmath>

namespace Lumina
{
    struct FColor
    {
        float R, G, B, A;
    
        FColor() : R(0.0f), G(0.0f), B(0.0f), A(1.0f) {}
        FColor(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}
    
        float GetR() const { return R; }
        float GetG() const { return G; }
        float GetB() const { return B; }
        float GetA() const { return A; }
    
        void SetR(float r) { R = r; }
        void SetG(float g) { G = g; }
        void SetB(float b) { B = b; }
        void SetA(float a) { A = a; }
    
        float ToGrayscale() const
        {
            return 0.2126f * R + 0.7152f * G + 0.0722f * B;
        }
    
        void Clamp()
        {
            R = std::clamp(R, 0.0f, 1.0f);
            G = std::clamp(G, 0.0f, 1.0f);
            B = std::clamp(B, 0.0f, 1.0f);
            A = std::clamp(A, 0.0f, 1.0f);
        }
    
        // Add two colors (component-wise)
        FColor operator+(const FColor& other) const
        {
            return FColor(R + other.R, G + other.G, B + other.B, A + other.A);
        }
    
        // Subtract two colors (component-wise)
        FColor operator-(const FColor& other) const
        {
            return FColor(R - other.R, G - other.G, B - other.B, A - other.A);
        }
    
        // Multiply color by a scalar
        FColor operator*(float scalar) const
        {
            return FColor(R * scalar, G * scalar, B * scalar, A * scalar);
        }
    
        // Multiply color by another color (component-wise)
        FColor operator*(const FColor& other) const
        {
            return FColor(R * other.R, G * other.G, B * other.B, A * other.A);
        }
    
        // Linear interpolation between two colors
        static FColor Lerp(const FColor& start, const FColor& end, float t)
        {
            return FColor(
                start.R + t * (end.R - start.R),
                start.G + t * (end.G - start.G),
                start.B + t * (end.B - start.B),
                start.A + t * (end.A - start.A)
            );
        }
    
        // Convert to string representation for easy logging or display
        std::string ToString() const
        {
            return "R: " + std::to_string(R) + " G: " + std::to_string(G) + 
                   " B: " + std::to_string(B) + " A: " + std::to_string(A);
        }
    
        // Utility function to create a color from a grayscale value
        static FColor FromGrayscale(float value, float alpha = 1.0f)
        {
            return FColor(value, value, value, alpha);
        }
    
        // Static helper function to convert RGB to HSL (Hue, Saturation, Lightness)
        static void RGBtoHSL(const FColor& color, float& h, float& s, float& l)
        {
            float r = color.R;
            float g = color.G;
            float b = color.B;
    
            float max = std::max({r, g, b});
            float min = std::min({r, g, b});
            float delta = max - min;
    
            h = 0.0f;
            if (delta != 0.0f) {
                if (max == r) {
                    h = (g - b) / delta;
                } else if (max == g) {
                    h = (b - r) / delta + 2.0f;
                } else {
                    h = (r - g) / delta + 4.0f;
                }
            }
    
            l = (max + min) / 2.0f;
            s = (max == min) ? 0.0f : (max - min) / (1.0f - std::abs(2.0f * l - 1.0f));
            h /= 6.0f; // Normalize hue to [0, 1]
        }
    
        // Static helper function to convert HSL to RGB
        static FColor HSLtoRGB(float h, float s, float l, float alpha = 1.0f)
        {
            float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
            float x = c * (1.0f - std::abs(fmod(h * 6.0f, 2.0f) - 1.0f));
            float m = l - c / 2.0f;
    
            float r = 0.0f, g = 0.0f, b = 0.0f;
    
            if (h < 1.0f / 6.0f)
            {
                r = c;
                g = x;
            }
            else if (h < 2.0f / 6.0f)
            {
                r = x;
                g = c;
            }
            else if (h < 3.0f / 6.0f)
            {
                g = c;
                b = x;
            }
            else if (h < 4.0f / 6.0f)
            {
                g = x;
                b = c;
            }
            else if (h < 5.0f / 6.0f)
            {
                r = x;
                b = c;
            }
            else
            {
                r = c;
                b = x;
            }
    
            return FColor((r + m), (g + m), (b + m), alpha);
        }
    };

}
