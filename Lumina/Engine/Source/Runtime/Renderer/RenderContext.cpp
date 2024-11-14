#include "RenderContext.h"

#include "RHI/Vulkan/VulkanRenderContext.h"


namespace Lumina
{
    FRenderContext* FRenderContext::Instance = nullptr;

    FRenderContext* FRenderContext::Create(const FRenderConfig& InConfig)
    {
        AssertMsg(Instance == nullptr, "Render Context Already Initialized!");
        Instance = new FVulkanRenderContext(InConfig);
        return Instance;
    }
    
}
