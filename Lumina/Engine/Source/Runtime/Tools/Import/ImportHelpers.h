#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Math/Math.h"
#include "Module/API.h"
#include "Platform/Platform.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    struct FMeshResource;
    class IRenderContext;
    struct FVertex;
}

namespace Lumina::Import
{
    namespace Textures
    {
        /** Gets an image's raw pixel data */
        LUMINA_API FIntVector2D ImportTexture(TVector<uint8>& OutPixels, const FString& RawFilePath, bool bFlipVertical = true);
    
        /** Creates a raw RHI Image */
        NODISCARD LUMINA_API FRHIImageRef CreateTextureFromImport(IRenderContext* RenderContext, const FString& RawFilePath, bool bFlipVerticalOnLoad = true);
    }


    namespace Mesh::GLTF
    {
        struct FGLTFMaterial
        {
            FName Name;
        };

        struct FGLTFImage
        {
            FString RelativePath;
            SIZE_T ByteOffset;
        };
        
        struct FGLTFImportData
        {
            TVector<FMeshResource> Resources;
            THashMap<SIZE_T, TVector<FGLTFMaterial>> Materials;
            TVector<FGLTFImage> Textures;
        };
        LUMINA_API void ImportGLTF(FGLTFImportData& OutData, const FString& RawFilePath);
    }

}
