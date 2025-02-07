#pragma once
#include "Containers/Array.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    namespace Fonts
    {
        void GetDecompressedFontData(uint8 const* pSourceData, TVector<uint8>& fontData );
    }
}
