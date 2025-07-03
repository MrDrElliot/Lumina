#include "ImportHelpers.h"

#include "stb_image/stb_image.h"

namespace Lumina::ImportHelpers
{
    FIntVector2D GetImagePixelData(TVector<uint8>& OutPixels, const FString& RawFilePath, bool bFlipVertical)
    {
        OutPixels.clear();
        
        stbi_set_flip_vertically_on_load(bFlipVertical);

        int x, y, c;
        stbi_uc* data = stbi_load(RawFilePath.c_str(), &x, &y, &c, STBI_rgb_alpha);
        if (data == nullptr)
        {
            LOG_WARN("Failed to get image pixel data from: {0}", RawFilePath);
            return {};
        }
        
        OutPixels.assign(data, data + static_cast<size_t>(x) * static_cast<size_t>(y) * STBI_rgb_alpha);
        stbi_image_free(data);
        
        return {x, y};
    }
    
}
