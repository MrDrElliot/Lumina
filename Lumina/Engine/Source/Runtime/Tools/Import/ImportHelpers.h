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

        struct FGLTFImportOptions
        {
            bool bImportMaterials = true;        // Whether to import materials defined in the GLTF file
            bool bImportTextures = true;         // Whether to import textures found in the GLTF file.
            bool bImportAnimations = true;       // Whether to import animations
            bool bGenerateTangents = true;       // Whether to generate tangents (if not present in file)
            bool bMergeMeshes = false;           // Whether to merge all meshes into a single static mesh
            bool bApplyTransforms = true;        // Whether to bake transforms into the mesh
            bool bUseCompression = false;        // Whether to compress vertex/index data after import
            float Scale = 1.0f;                  // Scene-wide scale factor
        };
        
        struct FGLTFImportData
        {
            TVector<FMeshResource> Resources;
            THashMap<SIZE_T, TVector<FGLTFMaterial>> Materials;
            TVector<FGLTFImage> Textures;
        };
        LUMINA_API void ImportGLTF(FGLTFImportData& OutData, const FGLTFImportOptions& ImportOptions, const FString& RawFilePath);
    }

}
