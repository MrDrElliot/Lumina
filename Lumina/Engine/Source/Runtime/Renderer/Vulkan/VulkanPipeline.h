#pragma once
#include <vector>

#include <vulkan/vulkan.h>

namespace Lumina
{
    class FVulkanPipeline
    {
    public:

        FVulkanPipeline() { Clear(); }
        void Clear();
        VkPipeline BuildPipeline(VkDevice InDevice);
        
        void SetShaders(VkShaderModule VertexShader, VkShaderModule FragmentShader);
        void SetInputTopology(VkPrimitiveTopology InTopology);
        void SetPolygonMode(VkPolygonMode InMode);
        void SetCullMode(VkCullModeFlags InCullMode, VkFrontFace InFrontFace);
        void SetMultisampingNone();
        void DisableBlending();
        void SetColorAttachmentFormat(VkFormat InFormat);
        void SetDepthFormat(VkFormat InFormat);
        void DisableDepthTest();
        void EnableDepthTest(bool bDepthWriteEnable, VkCompareOp InOp);
        void EnableBlendingAdditive();
        void EnableBlendingAlphablend();
        

        
        
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
   
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkPipelineRasterizationStateCreateInfo Rasterizer;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo Multisampling;
        VkPipelineLayout PipelineLayout;
        VkPipelineDepthStencilStateCreateInfo DepthStencil;
        VkPipelineRenderingCreateInfo RenderInfo;
        VkFormat ColorAttachmentformat;
        
        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;



        
    };
}
