#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Math/Math.h"
#include "Module/API.h"

namespace Lumina
{
    struct FVertex;
}

namespace Lumina::ImportHelpers
{
    LUMINA_API FIntVector2D GetImagePixelData(TVector<uint8>& OutPixels, const FString& RawFilePath, bool bFlipVertical = true);
    LUMINA_API void GetMeshVertexData(TVector<FVertex>& OutVertices, TVector<uint32>& OutIndices, const FString& RawFilePath);
}
