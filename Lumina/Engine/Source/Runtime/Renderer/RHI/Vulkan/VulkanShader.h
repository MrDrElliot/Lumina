#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina
{
    class FVulkanShader : public FShader
    {
    public:

        FVulkanShader(const TArray<FShaderData>& InData, const LString& Tag);
        ~FVulkanShader() override;

        TArray<VkPipelineShaderStageCreateInfo>& GetCreateInfos() { return StageCreateInfos; }
        TArray<VkDescriptorSetLayout>& GetLayouts() { return SetLayouts; }
        TArray<VkPushConstantRange>& GetRanges() { return Ranges; }

        virtual void SetFriendlyName(const LString& InString) override;
        
        void RestoreShaderModule(std::filesystem::path path) override;
        
        bool IsDirty() const override { return bDirty; }
        void SetDirty(bool NewDirty) override { bDirty = NewDirty; }

    private:
        
        TArray<VkPipelineShaderStageCreateInfo> StageCreateInfos;
        TArray<VkDescriptorSetLayout> SetLayouts;
        TArray<VkPushConstantRange> Ranges;
        std::filesystem::path Path;
        bool bDirty = false;
        
    
    };
}
