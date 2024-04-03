#include "RendererContext.h"

#include "Vulkan/VulkanRendererContext.h"


namespace Lumina
{
    LRendererContext* LRendererContext::Create()
    {
        LVulkanRendererContext* NewContext = new LVulkanRendererContext();
        NewContext->Init();
        return NewContext;
    }


    
}
