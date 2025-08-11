#pragma once

#include "VulkanDevice.h"
#include "VulkanMacros.h"
#include "VulkanRenderContext.h"
#include "Renderer/RenderResource.h"

namespace Lumina
{

    // ----------------------------------------------------------------------------------
    // GLSL / GLM Type       | Vulkan VkFormat Equivalent         | Size (Bytes) | Notes
    // ----------------------------------------------------------------------------------
    // float                 | VK_FORMAT_R32_SFLOAT               | 4            | 1 × 32-bit float
    // vec2 / glm::vec2      | VK_FORMAT_R32G32_SFLOAT            | 8            | 2 × 32-bit floats
    // vec3 / glm::vec3      | VK_FORMAT_R32G32B32_SFLOAT         | 12           | 3 × 32-bit floats (no padding in VkFormat)
    // vec4 / glm::vec4      | VK_FORMAT_R32G32B32A32_SFLOAT      | 16           | 4 × 32-bit floats
    // int                   | VK_FORMAT_R32_SINT                 | 4            | 1 × 32-bit signed integer
    // ivec2 / glm::ivec2    | VK_FORMAT_R32G32_SINT              | 8            | 2 × 32-bit signed integers
    // ivec3 / glm::ivec3    | VK_FORMAT_R32G32B32_SINT           | 12           | 3 × 32-bit signed integers
    // ivec4 / glm::ivec4    | VK_FORMAT_R32G32B32A32_SINT        | 16           | 4 × 32-bit signed integers
    // uint                  | VK_FORMAT_R32_UINT                 | 4            | 1 × 32-bit unsigned integer
    // uvec2 / glm::uvec2    | VK_FORMAT_R32G32_UINT              | 8            | 2 × 32-bit unsigned integers
    // uvec3 / glm::uvec3    | VK_FORMAT_R32G32B32_UINT           | 12           | 3 × 32-bit unsigned integers
    // uvec4 / glm::uvec4    | VK_FORMAT_R32G32B32A32_UINT        | 16           | 4 × 32-bit unsigned integers
    // ----------------------------------------------------------------------------------
    // Note:
    // - There is no VK_FORMAT for 3-element types with alignment padding like GLSL's std140 vec3.
    //   In vertex formats, VK_FORMAT_R32G32B32_* is valid and used, but memory alignment should
    //   still be handled carefully when packing structs in C++.
    // - Avoid using vec3 in UBOs unless you pad to vec4 alignment manually.
    // ----------------------------------------------------------------------------------


    
    class FVulkanSwapchain;

    struct FBufferChunk
    {
        FRHIBufferRef Buffer;
        uint64 Version = 0;
        uint64 BufferSize = 0;
        uint64 WritePointer = 0;
        void* MappedMemory = nullptr;

        static constexpr uint64 c_sizeAlignment = 4096; // GPU page size
    };

    class FUploadManager : public IDeviceChild
    {
    public:
        FUploadManager(FVulkanRenderContext* Ctx, uint64 InDefaultChunkSize, uint64 InMemoryLimit, bool bInIsScratchBuffer);

        TSharedPtr<FBufferChunk> CreateChunk(uint64 Size) const;

        bool SuballocateBuffer(uint64 Size, FRHIBuffer** Buffer, uint64* Offset, void** CpuVA, uint64 CurrentVersion, uint32 Alignment = 256);
        void SubmitChunks(uint64 CurrentVersion, uint64_t submittedVersion);

    private:
        
        FVulkanRenderContext* Context;
        uint64 DefaultChunkSize = 0;
        uint64 MemoryLimit = 0;
        uint64 AllocatedMemory = 0;
        bool bIsScratchBuffer = false;

        TList<TSharedPtr<FBufferChunk>>     ChunkPool;
        TSharedPtr<FBufferChunk>            CurrentChunk;
    };
    
    class FVulkanEventQuery : public IEventQuery
    {
    public:
        RENDER_RESOURCE(RRT_None)

        ECommandQueue   Queue = ECommandQueue::Graphics;
        uint64          CommandListID = 0;
        
    };
    
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

    // A copyable version of std::atomic.
    class FBufferVersionItem : public std::atomic<uint64> 
    {
    public:
        FBufferVersionItem()
        { }

        FBufferVersionItem(const FBufferVersionItem& other)
        {
            store(other);
        }

        FBufferVersionItem& operator=(const uint64 a)
        {
            store(a);
            return *this;
        }
    };

    class FVulkanBuffer : public FRHIBuffer, public IDeviceChild
    {
    public:
        friend class FVulkanCommandList;
        friend class FQueue;
        friend class FVulkanMemoryAllocator;
        
        FVulkanBuffer(FVulkanDevice* InDevice, const FRHIBufferDesc& InDescription);
        ~FVulkanBuffer() override;

        void* GetAPIResourceImpl(EAPIResourceType Type) override
        {
            return Buffer;
        }

        VkBuffer GetBuffer() const { return Buffer; }
        VmaAllocation GetAllocation() const { return Allocation; }
        void* GetMappedMemory() const;

    private:

        ECommandQueue               LastUseQueue = ECommandQueue::Graphics;
        uint64                      LastUseCommandListID = 0;

        TVector<FBufferVersionItem> VersionTracking;
        uint32                      VersionSearchStart = 0;
        VmaAllocation               Allocation = nullptr;
        VkBuffer                    Buffer = VK_NULL_HANDLE;

    };

    
    //----------------------------------------------------------------------------------------------


    class FVulkanSampler : public FRHISampler,  public IDeviceChild
    {
    public:

        FVulkanSampler(FVulkanDevice* InDevice, const FSamplerDesc& InDesc);
        ~FVulkanSampler() override;

        const FSamplerDesc& GetDesc() const override { return Desc; }

        void* GetAPIResourceImpl(EAPIResourceType Type) override { return Sampler; }

        
    private:

        FSamplerDesc    Desc;
        VkSampler       Sampler;
    };
    
    class FVulkanImage : public FRHIImage, public IDeviceChild
    {
    public:

        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription);
        FVulkanImage(FVulkanDevice* InDevice, const FRHIImageDesc& InDescription, VkImage RawImage, VkImageView RawView);
        ~FVulkanImage() override;

        void* GetAPIResourceImpl(EAPIResourceType Type) override;

        VkImage GetImage() const { return Image; }
        VkImageView GetImageView() const { return ImageView; }
        
        VkImageAspectFlags GetFullAspectMask() const { return FullAspectMask; }
        VkImageAspectFlags GetPartialAspectMask() const { return PartialAspectMask; }

    private:

        bool                        bImageManagedExternal = false;  // Mostly for swapchain.
        VkImageAspectFlags          FullAspectMask =        VK_IMAGE_ASPECT_NONE;
        VkImageAspectFlags          PartialAspectMask =     VK_IMAGE_ASPECT_NONE;
        VkImage                     Image =                 VK_NULL_HANDLE;
        VkImageView                 ImageView =             VK_NULL_HANDLE;
    };
    
    class IVulkanShader : public IDeviceChild
    {
    public:

        IVulkanShader(FVulkanDevice* InDevice, const TVector<uint32>& ByteCode, ERHIResourceType Type);
        
        ~IVulkanShader() override;

        void GetByteCodeImpl(const void** ByteCode, uint64* Size)
        {
            *ByteCode = SpirV.ByteCode.data();
            *Size = SpirV.ByteCode.size() * sizeof(uint32);
        }

        bool CompareSpirV(const IVulkanShader* Other) const
        {
            return VectorsAreEqual(Other->SpirV.ByteCode, SpirV.ByteCode);
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
    
        RENDER_RESOURCE(RTT_InputLayout)

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
        FVulkanBindingLayout(FVulkanDevice* InDevice, const FBindlessLayoutDesc& InDesc);

        ~FVulkanBindingLayout() override;

        bool Bake();
        const FBindingLayoutDesc* GetDesc() const override { return bBindless ? nullptr : &Desc; }
        const FBindlessLayoutDesc* GetBindlessDesc() const override { return bBindless ? &BindlessDesc : nullptr; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        
        FBindingLayoutDesc                      Desc;
        FBindlessLayoutDesc                     BindlessDesc;
        bool                                    bBindless = false;
        
        VkDescriptorSetLayout                   DescriptorSetLayout;
        TVector<VkDescriptorSetLayoutBinding>   Bindings;
        TVector<VkDescriptorPoolSize>           PoolSizes;
    };

    // Contains a VkDescriptorSet
    class FVulkanBindingSet : public FRHIBindingSet, public IDeviceChild
    {
    public:

        RENDER_RESOURCE(RRT_BindingSet)
        
        FVulkanBindingSet(FVulkanRenderContext* RenderContext, const FBindingSetDesc& InDesc, FVulkanBindingLayout* InLayout);
        ~FVulkanBindingSet() override;

        const FBindingSetDesc* GetDesc() const override { return &Desc; }
        FRHIBindingLayout* GetLayout() const override { return Layout; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;

        TVector<FRHIBufferRef>              DynamicBuffers;
        TVector<uint32>                     BindingsRequiringTransitions;
        TVector<FRHIResourceRef>            Resources;
        TRefCountPtr<FVulkanBindingLayout>  Layout;
        FBindingSetDesc                     Desc;
        VkDescriptorPool                    DescriptorPool;
        VkDescriptorSet                     DescriptorSet;
    };

    class FVulkanDescriptorTable : public FRHIDescriptorTable, public IDeviceChild
    {
    public:

        RENDER_RESOURCE(RRT_DescriptorTable)

        FVulkanDescriptorTable(FVulkanRenderContext* InContext, FVulkanBindingLayout* InLayout);
        ~FVulkanDescriptorTable() override;
        
        const FBindingSetDesc* GetDesc() const override { return nullptr; }
        FRHIBindingLayout* GetLayout() const override { return Layout; }
        uint32_t GetCapacity() const override { return Capacity; }

        uint32_t GetFirstDescriptorIndexInHeap() const override { return 0; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
        
        TRefCountPtr<FVulkanBindingLayout> Layout;
        uint32 Capacity = 0;

        VkDescriptorPool DescriptorPool = nullptr;
        VkDescriptorSet DescriptorSet = nullptr;
    };

    class FVulkanPipeline : public IDeviceChild
    {
    public:
        
        ~FVulkanPipeline() override;

        FVulkanPipeline(FVulkanDevice* InDevice)
            : IDeviceChild(InDevice)
            , PipelineLayout(nullptr)
            , Pipeline(nullptr)
            , PushConstantVisibility(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
        {
        }
        
        void CreatePipelineLayout(TVector<FRHIBindingLayoutRef> BindingLayouts, VkShaderStageFlags& OutStageFlags);
        
        VkPipelineLayout            PipelineLayout;
        VkPipeline                  Pipeline;
        VkShaderStageFlags          PushConstantVisibility;

    };
    
    class FVulkanGraphicsPipeline : public FRHIGraphicsPipeline,  public FVulkanPipeline
    {
    public:

        friend class FVulkanRenderContext;

        FVulkanGraphicsPipeline(FVulkanDevice* InDevice, const FGraphicsPipelineDesc& InDesc);

        const FGraphicsPipelineDesc& GetDesc() const override { return Desc; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
    
    private:

        FGraphicsPipelineDesc       Desc;
        
    };

    class FVulkanComputePipeline : public FRHIComputePipeline,  public FVulkanPipeline
    {
    public:

        friend class FVulkanRenderContext;

        FVulkanComputePipeline(FVulkanDevice* InDevice, const FComputePipelineDesc& InDesc);

        const FComputePipelineDesc& GetDesc() const override { return Desc; }
        void* GetAPIResourceImpl(EAPIResourceType Type) override;
    
    private:

        FComputePipelineDesc Desc;
    };
    
}
