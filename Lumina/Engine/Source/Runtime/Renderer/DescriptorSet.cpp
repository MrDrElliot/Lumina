#include "DescriptorSet.h"

#include "Renderer/RHIIncl.h"
#include "RHI/Vulkan/VulkanDescriptorSet.h"

namespace Lumina
{
    FRHIDescriptorSet FDescriptorSet::Create(const FDescriptorSetSpecification& InSpec)
    {
        return FRHIDescriptorSet(MakeRefCount<FVulkanDescriptorSet>(InSpec));
    }
    
}
