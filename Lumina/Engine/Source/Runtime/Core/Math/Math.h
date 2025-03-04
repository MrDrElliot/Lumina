#pragma once

#include <EASTL/algorithm.h>
#include <glm/glm.hpp>

#include "Core/Serialization/Archiver.h"

namespace Lumina
{
    class FMath
    {
    public:

    
    
    };
    
    template <typename T>
    struct TVector2D
    {
        T X, Y;

        // Default constructor
        TVector2D() : X(0), Y(0) {}

        TVector2D(T In) :X(In), Y(In) {}
        
        // Constructor with values
        TVector2D(T InX, T InY) : X(InX), Y(InY) {}

        // Copy constructor
        TVector2D(const TVector2D& Other) : X(Other.X), Y(Other.Y) {}

        // Assignment operator
        TVector2D& operator=(const TVector2D& Other)
        {
            if (this != &Other)
            {
                X = Other.X;
                Y = Other.Y;
            }
            return *this;
        }

        // Vector addition
        TVector2D operator+(const TVector2D& Other) const
        {
            return TVector2D(X + Other.X, Y + Other.Y);
        }

        bool operator==(const TVector2D& Other) const
        {
            return X == Other.X && Y == Other.Y;
        }

        // Vector subtraction
        TVector2D operator-(const TVector2D& Other) const
        {
            return TVector2D(X - Other.X, Y - Other.Y);
        }

        // Scalar multiplication
        TVector2D operator*(T Scalar) const
        {
            return TVector2D(X * Scalar, Y * Scalar);
        }

        // Scalar division
        TVector2D operator/(T Scalar) const
        {
            return TVector2D(X / Scalar, Y / Scalar);
        }

        // Dot product
        T Dot(const TVector2D& Other) const
        {
            return X * Other.X + Y * Other.Y;
        }

        // Length (magnitude) of the vector
        T Length() const
        {
            return std::sqrt(X * X + Y * Y);
        }

        // Normalized vector
        TVector2D Normalize() const
        {
            T Len = Length();
            return Len > 0 ? *this / Len : *this;
        }
        
        friend FArchive& operator << (FArchive& Ar, TVector2D& data)
        {
            Ar << data.X;
            Ar << data.Y;
            return Ar;
        }
    };

    template <typename T>
    struct TVector3D
    {
        T X, Y, Z;

        // Default constructor
        TVector3D() : X(0), Y(0), Z(0) {}

        TVector3D(T In) : X(In), Y(In), Z(In) {}
        
        // Constructor with values
        TVector3D(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) {}

        // Copy constructor
        TVector3D(const TVector3D& Other) : X(Other.X), Y(Other.Y), Z(Other.Z) {}

        // Assignment operator
        TVector3D& operator=(const TVector3D& Other)
        {
            if (this != &Other)
            {
                X = Other.X;
                Y = Other.Y;
                Z = Other.Z;
            }
            return *this;
        }

        bool operator==(const TVector3D& Other) const
        {
            return X == Other.X && Y == Other.Y && Z == Other.Z;
        }


        // Vector addition
        TVector3D operator+(const TVector3D& Other) const
        {
            return TVector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
        }

        // Vector subtraction
        TVector3D operator-(const TVector3D& Other) const
        {
            return TVector3D(X - Other.X, Y - Other.Y, Z - Other.Z);
        }

        // Scalar multiplication
        TVector3D operator*(T Scalar) const
        {
            return TVector3D(X * Scalar, Y * Scalar, Z * Scalar);
        }

        // Scalar division
        TVector3D operator/(T Scalar) const
        {
            return TVector3D(X / Scalar, Y / Scalar, Z / Scalar);
        }

        // Dot product
        T Dot(const TVector3D& Other) const
        {
            return X * Other.X + Y * Other.Y + Z * Other.Z;
        }

        // Cross product
        TVector3D Cross(const TVector3D& Other) const
        {
            return TVector3D(Y * Other.Z - Z * Other.Y,
                             Z * Other.X - X * Other.Z,
                             X * Other.Y - Y * Other.X);
        }

        // Length (magnitude) of the vector
        T Length() const
        {
            return std::sqrt(X * X + Y * Y + Z * Z);
        }

        // Normalized vector
        TVector3D Normalize() const
        {
            T Len = Length();
            return Len > 0 ? *this / Len : *this;
        }

        friend FArchive& operator << (FArchive& Ar, TVector3D& data)
        {
            Ar << data.X;
            Ar << data.Y;
            Ar << data.Z;
            return Ar;
        }
    };

    template <typename T>
    struct TVector4D
    {
        T X, Y, Z, W;

        // Default constructor
        TVector4D() : X(0), Y(0), Z(0), W(0) {}

        TVector4D(T In) :X(In), Y(In), Z(In), W(In){ }


        // Constructor with values
        TVector4D(T InX, T InY, T InZ, T InW) : X(InX), Y(InY), Z(InZ), W(InW) {}

        // Copy constructor
        TVector4D(const TVector4D& Other) : X(Other.X), Y(Other.Y), Z(Other.Z), W(Other.W) {}

        // Assignment operator
        TVector4D& operator=(const TVector4D& Other)
        {
            if (this != &Other)
            {
                X = Other.X;
                Y = Other.Y;
                Z = Other.Z;
                W = Other.W;
            }
            return *this;
        }

        // Vector addition
        TVector4D operator+(const TVector4D& Other) const
        {
            return TVector4D(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
        }

        // Vector subtraction
        TVector4D operator-(const TVector4D& Other) const
        {
            return TVector4D(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
        }

        // Scalar multiplication
        TVector4D operator*(T Scalar) const
        {
            return TVector4D(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
        }

        // Scalar division
        TVector4D operator/(T Scalar) const
        {
            return TVector4D(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
        }

        // Dot product
        T Dot(const TVector4D& Other) const
        {
            return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
        }

        // Length (magnitude) of the vector
        T Length() const
        {
            return std::sqrt(X * X + Y * Y + Z * Z + W * W);
        }

        // Normalized vector
        TVector4D Normalize() const
        {
            T Len = Length();
            return Len > 0 ? *this / Len : *this;
        }

        friend FArchive& operator << (FArchive& Ar, TVector4D& data)
        {
            Ar << data.X;
            Ar << data.Y;
            Ar << data.Z;
            Ar << data.W;
            return Ar;
        }
        
    };

    using FVector2D = TVector2D<float>;
    using FVector3D = TVector3D<float>;
    using FVector4D = TVector4D<float>;

    using FIntVector2D = TVector2D<int>;
    using FIntVector3D = TVector2D<int>;
    using FIntVector4D = TVector2D<int>;

}
