#include "Image.h"
#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanImage.h"

namespace Lumina
{
    FRHIImage FImage::Create(const FImageSpecification& Spec)
    {
        return FRHIImage(MakeRefCount<FVulkanImage>(Spec));
    }

    FRHIImageSampler FImageSampler::Create(const FImageSamplerSpecification& Spec)
    {
        return FRHIImageSampler(MakeRefCount<FVulkanImageSampler>(Spec));
    }
}
