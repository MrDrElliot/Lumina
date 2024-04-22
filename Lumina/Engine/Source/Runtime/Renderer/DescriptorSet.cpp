#include "DescriptorSet.h"

#include "RHI/Vulkan/VulkanDescriptorSet.h"

namespace Lumina
{
    std::shared_ptr<FDescriptorSet> FDescriptorSet::Create(const FDescriptorSetSpecification& InSpec)
    {
        return std::make_shared<FVulkanDescriptorSet>(InSpec);
    }
    
}
