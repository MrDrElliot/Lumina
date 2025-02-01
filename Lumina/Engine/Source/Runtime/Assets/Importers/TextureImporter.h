#pragma once

#include "AssetImporter.h"
#include <ThirdParty/stb_image/stb_image.h>

#include "Core/Performance/PerformanceTracker.h"
#include "Renderer/Image.h"


namespace Lumina
{
    class TextureImporter : public AssetImporter
    {
    public:

        bool Import(FArchive& Ar, void* ImportData, const FString& AssetPath) override
        {
            PROFILE_SCOPE_LOG(TextureImporter::Import)

            FImageSpecification* ImageSpec = static_cast<FImageSpecification*>(ImportData);
            if(ImageSpec == nullptr)
            {
                return false;
            }

            stbi_set_flip_vertically_on_load(true);

            int x, y, c;
            stbi_uc* data = stbi_load(AssetPath.c_str(), &x, &y, &c, STBI_rgb_alpha);
            AssertMsg(data, "Failed to import texture!");

            ImageSpec->Extent.x = x;
            ImageSpec->Extent.y = y;
            ImageSpec->SourceChannels = c;
            ImageSpec->Pixels.assign(data, data + (x * y * STBI_rgb_alpha));

            stbi_image_free(data); // Free the image data
            
            Ar << *ImageSpec;

            return true;
        }

    };
}
