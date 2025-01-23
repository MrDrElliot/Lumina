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

        FVulkanShader(const TFastVector<FShaderData>& InData, const LString& Tag);
        ~FVulkanShader() override;

        TFastVector<VkPipelineShaderStageCreateInfo>& GetCreateInfos() { return StageCreateInfos; }
        TFastVector<VkDescriptorSetLayout>& GetLayouts() { return SetLayouts; }
        TFastVector<VkPushConstantRange>& GetRanges() { return Ranges; }

        virtual void SetFriendlyName(const LString& InString) override;
        
        void RestoreShaderModule(std::filesystem::path path) override;
        
        bool IsDirty() const override { return bDirty; }
        void SetDirty(bool NewDirty) override { bDirty = NewDirty; }

    private:
        
        TFastVector<VkPipelineShaderStageCreateInfo> StageCreateInfos;
        TFastVector<VkDescriptorSetLayout> SetLayouts;
        TFastVector<VkPushConstantRange> Ranges;
        std::filesystem::path Path;
        bool bDirty = false;
        
    
    };
}
