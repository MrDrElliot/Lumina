#include "DescriptorSet.h"

#include "RHI/Vulkan/VulkanDescriptorSet.h"

namespace Lumina
{
    TRefPtr<FDescriptorSet> FDescriptorSet::Create(const FDescriptorSetSpecification& InSpec)
    {
        return MakeRefPtr<FVulkanDescriptorSet>(InSpec);
    }
    
}
