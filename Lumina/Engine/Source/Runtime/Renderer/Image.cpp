#include "Image.h"

#include "RHI/Vulkan/VulkanImage.h"

namespace Lumina
{
    std::shared_ptr<FImage> FImage::Create(const FImageSpecification& Spec)
    {
        return std::make_shared<FVulkanImage>(Spec);
    }

    std::shared_ptr<FImageSampler> FImageSampler::Create(const FImageSamplerSpecification& Spec)
    {
        return std::make_shared<FVulkanImageSampler>(Spec);
    }
}
