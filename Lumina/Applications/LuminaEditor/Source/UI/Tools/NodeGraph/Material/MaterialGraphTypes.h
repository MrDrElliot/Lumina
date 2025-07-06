#pragma once
#include "Containers/String.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    enum class EMaterialInputType : uint8
    {
        Float,
        Float2,
        Float3,
        Float4,
        Texture,
        Wildcard,
    };

    enum class EComponentMask : uint8
    {
        RGBA,
        R,
        G,
        B,
        A,
        RG,
        GB,
        RGB,
    };

    inline FString GetSwizzleForMask(EComponentMask Mask)
    {
        switch (Mask)
        {
        case EComponentMask::R:    return ".r";
        case EComponentMask::G:    return ".g";
        case EComponentMask::B:    return ".b";
        case EComponentMask::A:    return ".a";
        case EComponentMask::RG:   return ".rg";
        case EComponentMask::GB:   return ".gb";
        case EComponentMask::RGB:  return ".rgb";
        case EComponentMask::RGBA: return ""; // no swizzle needed
        default:                   return "";
        }
    }

    inline FString FixupComponentSwizzle(EComponentMask Mask, const FString& Node)
    {
        switch (Mask)
        {
        case EComponentMask::R:
            return "vec4(" + Node + ".r, " + Node + ".r, " + Node + ".r, " + Node + ".r)";
        case EComponentMask::G:
            return "vec4(" + Node + ".g, " + Node + ".g, " + Node + ".g, " + Node + ".g)";
        case EComponentMask::B:
            return "vec4(" + Node + ".b, " + Node + ".b, " + Node + ".b, " + Node + ".b)";
        case EComponentMask::A:
            return "vec4(" + Node + ".a, " + Node + ".a, " + Node + ".a, " + Node + ".a)";
        case EComponentMask::RG:
            return "vec4(" + Node + ".r, " + Node + ".g, 0.0, " + Node + ".a)";
        case EComponentMask::GB:
            return "vec4(0.0, " + Node + ".g, " + Node + ".b, " + Node + ".a)";
        case EComponentMask::RGB:
            return "vec4(" + Node + ".r, " + Node + ".g, " + Node + ".b, " + Node + ".a)";
        case EComponentMask::RGBA:
            return Node; // Already full vec4, no fixup needed
        default:
            return Node;
        }
    }
}   