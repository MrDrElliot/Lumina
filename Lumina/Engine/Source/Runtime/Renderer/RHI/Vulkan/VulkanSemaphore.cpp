
#include "VulkanSemaphore.h"
#include "Renderer/RHIIncl.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"


namespace Lumina
{
    FVulkanSemaphore::FVulkanSemaphore()
    {
        VkSemaphoreCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        FVulkanRenderContext* Context = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = Context->GetDevice();
        
        VK_CHECK(vkCreateSemaphore(Device, &CreateInfo, nullptr, &Semaphore));
    }

    FVulkanSemaphore::~FVulkanSemaphore()
    {
        FVulkanRenderContext* Context = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = Context->GetDevice();
        
        vkDestroySemaphore(Device, Semaphore, nullptr);
    }

    void FVulkanSemaphore::SetFriendlyName(const FString& InName)
    {
        FVulkanRenderContext* Context = FRenderer::GetRenderContext<FVulkanRenderContext>();
        VkDevice Device = Context->GetDevice();
        ;
        VkDebugUtilsObjectNameInfoEXT NameInfo = {};
        NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        NameInfo.pObjectName = InName.c_str();
        NameInfo.objectType = VK_OBJECT_TYPE_SEMAPHORE;
        NameInfo.objectHandle = reinterpret_cast<uint64_t>(Semaphore);

        Context->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
    }
}
