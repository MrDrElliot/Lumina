#pragma once
#include "Source/Runtime/Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Lumina
{
    class FVulkanShader : public FShader
    {
    public:

        FVulkanShader(std::map<EShaderStage, std::vector<glm::uint32>> Binaries, std::filesystem::path Path);
        ~FVulkanShader() override;

        
        std::vector<VkPipelineShaderStageCreateInfo> GetCreateInfos() const { return StageCreateInfos; }
        std::vector<VkDescriptorSetLayout> GetLayouts() const { return SetLayouts; }
        std::vector<VkPushConstantRange> GetRanges() const { return Ranges; }

        void RestoreShaderModule(std::filesystem::path path) override;
        void Destroy() override;
        
        bool IsDirty() const override { return bDirty; }
        void SetDirty(bool NewDirty) override { bDirty = NewDirty; }

    private:
        
        std::vector<VkPipelineShaderStageCreateInfo> StageCreateInfos;
        std::vector<VkDescriptorSetLayout> SetLayouts;
        std::vector<VkPushConstantRange> Ranges;
        std::filesystem::path Path;
        bool bDirty = false;
        
    
    };
}
