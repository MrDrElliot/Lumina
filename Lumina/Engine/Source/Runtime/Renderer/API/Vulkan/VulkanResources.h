#pragma once

#include "VulkanDevice.h"
#include "VulkanMacros.h"
#include "Renderer/RenderResource.h"

namespace Lumina
{

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
        
        VkImageAspectFlags          FullAspectMask =    VK_IMAGE_ASPECT_NONE;
        VkImageAspectFlags          PartialAspectMask = VK_IMAGE_ASPECT_NONE;
        VkImage                     Image =             VK_NULL_HANDLE;
        VkImageView                 ImageView =         VK_NULL_HANDLE;
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
        
        struct FSpirvCode
        {
            TVector<uint32> ByteCode;
        } SpirV;
        

        VkShaderModule  ShaderModule = VK_NULL_HANDLE;
    };

    
    
    class FRHIVulkanVertexShader : public FRHIVertexShader, public IVulkanShader
    {
    public:
        RENDER_RESOURCE(RRT_VertexShader)

        FRHIVulkanVertexShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_VertexShader)
        {}
    };

    class FRHIVulkanPixelShader : public FRHIPixelShader, public IVulkanShader
    {
    public:

        RENDER_RESOURCE(RRT_PixelShader)

        FRHIVulkanPixelShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_PixelShader)
        {}
    };

    class FRHIVulkanComputeShader : public FRHIComputeShader, public IVulkanShader
    {
    public:
        RENDER_RESOURCE(RRT_ComputeShader)

        FRHIVulkanComputeShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode)
            :IVulkanShader(InDevice, ByteCode, RRT_ComputeShader)
        {}
    };
    
}
