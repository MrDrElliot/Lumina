#include "Image.h"
#include "Swapchain.h"
#include "RHI/Vulkan/VulkanImage.h"

namespace Lumina
{
    TRefPtr<FImage> FImage::Create(const FImageSpecification& Spec)
    {
        return MakeRefPtr<FVulkanImage>(Spec);
    }

    TRefPtr<FImageSampler> FImageSampler::Create(const FImageSamplerSpecification& Spec)
    {
        return MakeRefPtr<FVulkanImageSampler>(Spec);
    }
}
