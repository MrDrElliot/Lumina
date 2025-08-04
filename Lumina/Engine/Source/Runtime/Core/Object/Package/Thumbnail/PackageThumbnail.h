#pragma once
#include "Containers/Array.h"
#include "Core/Serialization/Archiver.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    struct FPackageThumbnail
    {
        uint32 ImageWidth = 0;
        uint32 ImageHeight = 0;
        TVector<uint8> ImageData;

        void Serialize(FArchive& Ar)
        {
            Ar << ImageWidth;
            Ar << ImageHeight;
            Ar << ImageData;
        }
    };
}
