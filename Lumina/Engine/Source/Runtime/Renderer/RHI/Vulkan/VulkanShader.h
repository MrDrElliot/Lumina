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

        FVulkanShader(const TVector<FShaderData>& InData, const FString& Tag);
        ~FVulkanShader() override;

        TVector<VkPipelineShaderStageCreateInfo>& GetCreateInfos() { return StageCreateInfos; }
        TVector<VkDescriptorSetLayout>& GetLayouts() { return SetLayouts; }
        TVector<VkPushConstantRange>& GetRanges() { return Ranges; }

        virtual void SetFriendlyName(const FString& InString) override;
        
        void RestoreShaderModule(std::filesystem::path path) override;
        
        bool IsDirty() const override { return bDirty; }
        void SetDirty(bool NewDirty) override { bDirty = NewDirty; }

    private:
        
        TVector<VkPipelineShaderStageCreateInfo> StageCreateInfos;
        TVector<VkDescriptorSetLayout> SetLayouts;
        TVector<VkPushConstantRange> Ranges;
        std::filesystem::path Path;
        bool bDirty = false;
        
    
    };
}
