#include "RendererContext.h"

#include "Vulkan/VulkanRendererContext.h"


namespace Lumina
{
    LRendererContext* LRendererContext::Create(bool bInit)
    {
        LVulkanRendererContext* NewContext = new LVulkanRendererContext();
        if(bInit)
        {
            NewContext->Init();
        }
        return NewContext;
    }


    
}
