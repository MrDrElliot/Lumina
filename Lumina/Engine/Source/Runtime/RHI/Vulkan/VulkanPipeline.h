#pragma once


#include "Source/Runtime/Log/Log.h"
#include <string>
#include <fstream>
#include "VulkanRenderPass.h"

namespace Lumina
{
    class FVulkanPipeline
    {
    public:
        FVulkanPipeline(VkDevice InDevice, FVulkanSwapChain* InSwapChain, FVulkanRenderPass* InRenderPass);
        ~FVulkanPipeline();

        VkShaderModule CreateShaderModule(const std::string& InCode) const;

        VkPipeline GetPipeline() const { return GraphicsPipeline; }
        

        static std::string ReadFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);
            if (file.is_open())
            {
                LE_LOG_TRACE("Found File!");
                const size_t fileSize = static_cast<size_t>(file.tellg());
                file.seekg(0);

                std::string buffer(fileSize, '\0');
                file.read(buffer.data(), fileSize);

                file.close();
                return buffer;
            }
            LE_LOG_TRACE("Failed to find file!");

            return "";
        }

    private:
        VkPipeline GraphicsPipeline;

        VkDevice Device;
        FVulkanSwapChain* SwapChain;
        VkPipelineLayout Layout;
        FVulkanRenderPass* RenderPass;

    };
}
