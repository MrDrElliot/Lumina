#include "VulkanPipeline.h"

#include "VulkanHelpers.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    VkPipeline FVulkanPipeline::BuildPipeline(VkDevice InDevice)
    {
        VkPipelineViewportStateCreateInfo ViewportState = {};
        ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ViewportState.pNext = nullptr;

        ViewportState.viewportCount = 1;
        ViewportState.scissorCount =1;


        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;

        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &ColorBlendAttachment;


        VkPipelineVertexInputStateCreateInfo VertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

        VkGraphicsPipelineCreateInfo PipelineInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        PipelineInfo.pNext = &RenderInfo;
        PipelineInfo.stageCount = (uint32_t)ShaderStages.size();
        PipelineInfo.pStages = ShaderStages.data();
        PipelineInfo.pVertexInputState = &VertexInputInfo;
        PipelineInfo.pInputAssemblyState = &InputAssembly;
        PipelineInfo.pViewportState = &ViewportState;
        PipelineInfo.pRasterizationState = &Rasterizer;
        PipelineInfo.pMultisampleState = &Multisampling;
        PipelineInfo.pColorBlendState = &colorBlending;
        PipelineInfo.pDepthStencilState = &DepthStencil;
        PipelineInfo.layout = PipelineLayout;


        VkDynamicState State[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo DynamicInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        DynamicInfo.pDynamicStates = &State[0];
        DynamicInfo.dynamicStateCount = 2;
        PipelineInfo.pDynamicState = &DynamicInfo;

        VkPipeline NewPipeline;
        if(vkCreateGraphicsPipelines(InDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &NewPipeline) != VK_SUCCESS)
        {
            LE_LOG_CRITICAL("Failed to create pipeline!");
            return VK_NULL_HANDLE;
        }
        return NewPipeline;
    }

    void FVulkanPipeline::SetShaders(VkShaderModule VertexShader, VkShaderModule FragmentShader)
    {
        ShaderStages.clear();

        ShaderStages.push_back(Vulkan::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, VertexShader));

        ShaderStages.push_back(Vulkan::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, FragmentShader));
    }

    void FVulkanPipeline::SetInputTopology(VkPrimitiveTopology InTopology)
    {
        InputAssembly.topology = InTopology;

        InputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void FVulkanPipeline::SetPolygonMode(VkPolygonMode InMode)
    {
        Rasterizer.polygonMode = InMode;
        Rasterizer.lineWidth = 1.f;
    }

    void FVulkanPipeline::SetCullMode(VkCullModeFlags InCullMode, VkFrontFace InFrontFace)
    {
        Rasterizer.cullMode = InCullMode;
        Rasterizer.frontFace = InFrontFace;
    }

    void FVulkanPipeline::SetMultisampingNone()
    {
        Multisampling.sampleShadingEnable = VK_FALSE;
        Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        Multisampling.minSampleShading = 1.0f;
        Multisampling.pSampleMask = nullptr;
        Multisampling.alphaToCoverageEnable = VK_FALSE;
        Multisampling.alphaToOneEnable = VK_FALSE;
    }

    void FVulkanPipeline::DisableBlending()
    {
        ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        ColorBlendAttachment.blendEnable = VK_FALSE;
    }

    void FVulkanPipeline::SetColorAttachmentFormat(VkFormat InFormat)
    {
        ColorAttachmentformat = InFormat;
        RenderInfo.colorAttachmentCount = 1;
        RenderInfo.pColorAttachmentFormats = &ColorAttachmentformat;
    }

    void FVulkanPipeline::SetDepthFormat(VkFormat InFormat)
    {
        RenderInfo.depthAttachmentFormat = InFormat;
    }

    void FVulkanPipeline::DisableDepthTest()
    {
        DepthStencil.depthTestEnable = VK_FALSE;
        DepthStencil.depthWriteEnable = VK_FALSE;
        DepthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
        DepthStencil.depthBoundsTestEnable = VK_FALSE;
        DepthStencil.stencilTestEnable = VK_FALSE;
        DepthStencil.front = {};
        DepthStencil.back = {};
        DepthStencil.minDepthBounds = 0.f;
        DepthStencil.maxDepthBounds= 1.f;
    }

    void FVulkanPipeline::Clear()
    {
    
        InputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

        Rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

        ColorBlendAttachment = {};

        Multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

        PipelineLayout = {};

        DepthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        RenderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

        ShaderStages.clear();
    }
}