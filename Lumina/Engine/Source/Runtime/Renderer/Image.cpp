#include "Image.h"

#include "RHI/Vulkan/VulkanImage.h"

namespace Lumina
{
    FImage* FImage::Create(const FImageSpecification& Spec)
    {
        return new FVulkanImage(Spec);
    }

    FImageSampler* FImageSampler::Create(const FImageSamplerSpecification& Spec)
    {
        return new FVulkanImageSampler(Spec);
    }
}
