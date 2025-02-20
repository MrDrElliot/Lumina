#pragma once

#include <vulkan/vulkan.h>
#include "Lumina.h"
#include "Renderer/Shader.h"

namespace Lumina
{

    struct FVulkanShaderReflctionData
    {
        TVector<VkPipelineShaderStageCreateInfo>        StageCreateInfos;
        TVector<VkDescriptorSetLayout>                  SetLayouts;
        TVector<VkPushConstantRange>                    Ranges;
    };
    
    class FVulkanShader : public FShader
    {
    public:

        FVulkanShader()
            : VkReflectionData()
        {}

        ~FVulkanShader() override;
        
        const void* GetPlatformReflectionData() const override { return &VkReflectionData; }

        void CreateStage(const FShaderStage& StageData) override;

        void GeneratePlatformShaderStageReflectionData(const FShaderReflectionData& ReflectionData) override;

        virtual void SetFriendlyName(const FString& InString) override;
    
    private:
        
        FVulkanShaderReflctionData VkReflectionData;
        
    };
}
