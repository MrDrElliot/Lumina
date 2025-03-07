#pragma once

#include "VulkanDevice.h"
#include "VulkanMacros.h"
#include "Renderer/RenderResource.h"

namespace Lumina
{
    class FVulkanSwapchain;

    class FVulkanViewport : public FRHIViewport
    {
    public:
        
        friend class FVulkanRenderContext;
        

        FVulkanViewport(const FIntVector2D& InSize, IRenderContext* InContext)
            : FRHIViewport(InSize, InContext)
        {}

    private:

        FVulkanSwapchain* Swapchain = nullptr;
        
    };

    class FVulkanBuffer : public FRHIBuffer, public IDeviceChild
    {
    public:

        FVulkanBuffer(FVulkanDevice* InDevice, const FRHIBufferDesc& InDescription);
        ~FVulkanBuffer() override;


        FORCEINLINE VkBuffer GetBuffer() const { return Buffer; }
        

    private:
        
        VkBuffer Buffer = VK_NULL_HANDLE;
    };

    
    //----------------------------------------------------------------------------------------------


    class FVulkanImage : public FRHIImage, public IDeviceChild
    {
    public:

        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription);
        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription, VkImage RawImage, VkImageView RawView);
        ~FVulkanImage() override;

        void* GetAPIResourceImpl(EAPIResourceType Type) override;

        VkImage GetImage() const { return Image; }
        VkImageView GetImageView() const { return ImageView; }
        
        FORCEINLINE VkImageAspectFlags GetFullAspectMask() const { return FullAspectMask; }
        FORCEINLINE VkImageAspectFlags GetPartialAspectMask() const { return PartialAspectMask; }

    private:

        bool                        bImageManagedExternal = false; // Mostly for swapchain.
        VkImageAspectFlags          FullAspectMask =        VK_IMAGE_ASPECT_NONE;
        VkImageAspectFlags          PartialAspectMask =     VK_IMAGE_ASPECT_NONE;
        VkImage                     Image =                 VK_NULL_HANDLE;
        VkImageView                 ImageView =             VK_NULL_HANDLE;
    };


    
    
    class IVulkanShader : public IDeviceChild
    {
    public:

        IVulkanShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode, ERHIResourceType Type)
            :IDeviceChild(InDevice)
        {
            Assert(!ByteCode.empty());
            
            SpirV.ByteCode = ByteCode;

            VkShaderModuleCreateFlags Flags = 0;
            
            VkShaderModuleCreateInfo CreateInfo = {};
            CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            CreateInfo.codeSize = ByteCode.size() * sizeof(uint32);
            CreateInfo.pCode = ByteCode.data();
            CreateInfo.flags = Flags;

            VK_CHECK(vkCreateShaderModule(Device->GetDevice(), &CreateInfo, nullptr, &ShaderModule));
        }
        
        ~IVulkanShader()
        {
            vkDestroyShaderModule(Device->GetDevice(), ShaderModule, nullptr);
        }

        void GetByteCodeImpl(const void** ByteCode, uint64* Size)
        {
            *ByteCode = SpirV.ByteCode.data();
            *Size = SpirV.ByteCode.size() * sizeof(uint32);
        }

    protected:
        
        struct FSpirvCode
        {
            TVector<uint32> ByteCode;
        } SpirV;
        
        
        VkShaderModule  ShaderModule = VK_NULL_HANDLE;
    };

    
    
    class FVulkanVertexShader : public FRHIVertexShader, public IVulkanShader
    {
    public:
        RENDER_RESOURCE(RRT_VertexShader)

        FVulkanVertexShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_VertexShader)
        {}

        void* GetAPIResourceImpl(EAPIResourceType ResourceType) override
        {
            return ShaderModule;
        }
        
        void GetByteCode(const void** ByteCode, uint64* Size) override
        {
            GetByteCodeImpl(ByteCode, Size);
        }
    };

    class FVulkanPixelShader : public FRHIPixelShader, public IVulkanShader
    {
    public:

        RENDER_RESOURCE(RRT_PixelShader)

        FVulkanPixelShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_PixelShader)
        {}

        void* GetAPIResourceImpl(EAPIResourceType ResourceType) override
        {
            return ShaderModule;
        }
        
        void GetByteCode(const void** ByteCode, uint64* Size) override
        {
            GetByteCodeImpl(ByteCode, Size);
        }
    };

    class FVulkanComputeShader : public FRHIComputeShader, public IVulkanShader
    {
    public:
        RENDER_RESOURCE(RRT_ComputeShader)

        FVulkanComputeShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_ComputeShader)
        {}

        void* GetAPIResourceImpl(EAPIResourceType ResourceType) override
        {
            return ShaderModule;
        }
        
        void GetByteCode(const void** ByteCode, uint64* Size) override
        {
            GetByteCodeImpl(ByteCode, Size);
        }
    };

    class FVulkanInputLayout : public IRHIInputLayout
    {
    public:
    
        RENDER_RESOURCE(RRT_None);

        FVulkanInputLayout(const FVertexAttributeDesc* InAttributeDesc, uint32 AttributeCount);
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        
        
        TVector<FVertexAttributeDesc> InputDesc;
        TVector<VkVertexInputBindingDescription> BindingDesc;
        TVector<VkVertexInputAttributeDescription> AttributeDesc;
        
        uint32 GetNumAttributes() const override;
        const FVertexAttributeDesc* GetAttributeDesc(uint32 index) const override;
    };


    class FVulkanBindingLayout : public FRHIBindingLayout, public IDeviceChild
    {
    public:

        RENDER_RESOURCE(RRT_BindingLayout)

        FVulkanBindingLayout(FVulkanDevice* InDevice, const FBindingLayoutDesc& InDesc);
        ~FVulkanBindingLayout() override;
        
        const FBindingLayoutDesc* GetDesc() const override { return &Desc; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        
        FBindingLayoutDesc                      Desc;
        VkDescriptorSetLayout                   DescriptorSetLayout;
        TVector<VkDescriptorSetLayoutBinding>   Bindings;
        TVector<VkDescriptorPoolSize>           PoolSizes;
    };

    class FVulkanBindingSet : public FRHIBindingSet, public IDeviceChild
    {
    public:

        RENDER_RESOURCE(RRT_BindingSet)
        
        FVulkanBindingSet(FVulkanDevice* InDevice, const FBindingSetDesc& InDesc, FVulkanBindingLayout* InLayout);
        ~FVulkanBindingSet() override;

        const FBindingSetDesc* GetDesc() const override { return &Desc; }
        FRHIBindingLayout* GetLayout() const override { return Layout; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;

        TVector<uint32>                     BindingsRequiringTransitions;
        TVector<FRHIResourceRef>            Resources;
        TRefCountPtr<FVulkanBindingLayout>  Layout;
        FBindingSetDesc                     Desc;
        VkDescriptorPool                    DescriptorPool;
        VkDescriptorSet                     DescriptorSet;
    };
    

    class FVulkanPipeline : public IDeviceChild
    {
    public:
        
        virtual ~FVulkanPipeline() = default;

        FVulkanPipeline(FVulkanDevice* InDevice)
            :IDeviceChild(InDevice)
            , PipelineLayout(nullptr)
            , Pipeline(nullptr)
        {}

        virtual void Bind(VkCommandBuffer CmdBuffer) = 0;

        void CreatePipelineLayout(TVector<FRHIBindingLayoutRef> BindingLayouts);
        
        VkPipelineLayout PipelineLayout;
        VkPipeline Pipeline;
    };
    
    class FVulkanGraphicsPipeline : public FRHIGraphicsPipeline,  public FVulkanPipeline
    {
    public:

        friend class FVulkanRenderContext;

        FVulkanGraphicsPipeline(FVulkanDevice* InDevice, const FGraphicsPipelineDesc& InDesc);

        const FGraphicsPipelineDesc& GetDesc() const override { return Desc; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override { return Pipeline; }

        void Bind(VkCommandBuffer CmdBuffer) override
        {
            vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
        }

    private:

        FGraphicsPipelineDesc Desc;
    };

    class FVulkanComputePipeline : public FRHIComputePipeline,  public FVulkanPipeline
    {
    public:

        friend class FVulkanRenderContext;

        FVulkanComputePipeline(FVulkanDevice* InDevice, const FComputePipelineDesc& InDesc);

        const FComputePipelineDesc& GetDesc() const override { return Desc; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override { return Pipeline; }

        void Bind(VkCommandBuffer CmdBuffer) override
        {
            vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipeline);
        }

    private:

        FComputePipelineDesc Desc;
    };
    
}
