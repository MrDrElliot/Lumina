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
            SpirV.ByteCode = ByteCode;

            VkShaderModuleCreateFlags Flags = 0;
            
            VkShaderModuleCreateInfo CreateInfo = {};
            CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            CreateInfo.codeSize = ByteCode.size();
            CreateInfo.pCode = ByteCode.data();
            CreateInfo.pNext = nullptr;
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

        void* GetAPIResourceImpl(EAPIResourceType Type) override
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

        void* GetAPIResourceImpl(EAPIResourceType Type) override
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

        void* GetAPIResourceImpl(EAPIResourceType Type) override
        {
            return ShaderModule;
        }
        
        void GetByteCode(const void** ByteCode, uint64* Size) override
        {
            GetByteCodeImpl(ByteCode, Size);
        }
    };

    class FVulkanGraphicsPipeline : public FRHIGraphicsPipeline
    {
    public:

        FVulkanGraphicsPipeline(const FGraphicsPipelineDesc& InDesc);
        void* GetAPIResourceImpl(EAPIResourceType Type) override
        {
            return Pipeline;
        }

    private:

        VkPipeline Pipeline;
    };

    class FVulkanComputePipeline : public FRHIComputePipeline
    {
    public:

        FVulkanComputePipeline(const FComputePipelineDesc& InDesc);
        void* GetAPIResourceImpl(EAPIResourceType Type) override
        {
            return Pipeline;
        }

    private:

        VkPipeline Pipeline;
        
    };
    
}
