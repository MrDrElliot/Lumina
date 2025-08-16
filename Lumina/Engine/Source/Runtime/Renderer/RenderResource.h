#pragma once

#include <vulkan/vulkan.h>

#include "Format.h"
#include "RenderTypes.h"
#include "StateTracking.h"
#include "ViewVolume.h"
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Core/Threading/Thread.h"
#include "Memory/RefCounted.h"
#include "Types/BitFlags.h"


namespace Lumina
{
	enum class EBufferUsageFlags : uint32;
    class IRenderContext;
}

    static constexpr uint32 MaxRenderTargets = 8;
	static constexpr uint32 MaxVertexAttributes = 16;
	static constexpr uint32 MaxPushConstantSize = 128; // D3D12: root signature is 256 bytes max., Vulkan: 128 bytes of push constants guaranteed
	static constexpr uint32 MaxBindingLayouts = 4;
	static constexpr uint32 MaxBindingsPerLayout = 128;


enum class EFormatKind : uint8
{
	Integer,
	Normalized,
	Float,
	DepthStencil
};

struct FFormatInfo
{
	EFormat Format;
	const char* Name;
	uint8 BytesPerBlock;
	uint8 BlockSize;
	EFormatKind Kind;
	uint8 bHasRed : 1;
	uint8 bHasGreen : 1;
	uint8 bHasBlue : 1;
	uint8 bHasAlpha : 1;
	uint8 bHasDepth : 1;
	uint8 bHasStencil : 1;
	uint8 bIsSigned : 1;
	uint8 bIsSRGB : 1;
};

const FFormatInfo& GetFormatInfo(EFormat format);


/** An enumeration of the different RHI reference types. */
enum ERHIResourceType : uint8
{
	RRT_None,

	RRT_SamplerState,
	RTT_InputLayout,
	RRT_BindingLayout,
	RRT_BindingSet,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexShader,
	RRT_PixelShader,
	RRT_ComputeShader,
	RRT_ShaderLibrary,
	RRT_GraphicsPipeline,
	RRT_ComputePipeline,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
	RRT_Image,
	RRT_GPUFence,
	RRT_Viewport,
	RRT_StagingBuffer,
	RRT_CommandList,
	RRT_DescriptorTable,

	RRT_Num
};

enum class ERHIBindingResourceType : uint8
{
	// SRV (Shader Resource View) Types.
	Texture_SRV,
	Buffer_SRV,

	Sampler,

	// UAV (Unordered Access View) Types.
	Texture_UAV,
	Buffer_UAV,

	// Constant Buffer (could be a uniform buffer)
	Buffer_CBV,
	Buffer_Uniform = Buffer_CBV,
	Buffer_Uniform_Dynamic,
	Buffer_Storage_Dynamic,

	// Push Constant Ranges
	PushConstants,
};

enum class ERHIBindingPoint : uint8
{
	Graphics,
	Compute,
};

ENUM_CLASS_FLAGS(ERHIResourceType)


enum class ERHIShaderType : uint8
{
	None =		ERHIResourceType::RRT_None,
	Vertex =	ERHIResourceType::RRT_VertexShader,
	Fragment =	ERHIResourceType::RRT_PixelShader,
	Compute =	ERHIResourceType::RRT_ComputeShader,
};

ENUM_CLASS_FLAGS(ERHIShaderType)

enum class EAPIResourceType : uint8
{
	Default = 0,
	Buffer,
	Image,
	ImageView,
	Sampler,
	ShaderModule,
	Pipeline,
	PipelineLayout,
	RenderPass,
	Framebuffer,
	DescriptorSet,
	DescriptorSetLayout,
	DescriptorPool,
	CommandPool,
	CommandBuffer,
	Semaphore,
	Fence,
	Event,
	QueryPool,
	DeviceMemory,
	Swapchain,
	Surface,
	Device,
	Instance,
	Queue
};

enum class EPrimitiveType : uint8
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	TriangleListWithAdjacency,
	TriangleStripWithAdjacency,
	PatchList
};

#define RENDER_RESOURCE(TypeValue) \
static constexpr ERHIResourceType Type = ERHIResourceType::TypeValue; \
static constexpr ERHIResourceType GetStaticResourceType() { return ERHIResourceType::TypeValue; } \
virtual ERHIResourceType GetResourceType() const override { return ERHIResourceType:: TypeValue; }

/**
 * Base class for all render resources. Reference counted and destroyed externally by the implementation.
 *
 */

namespace Lumina
{

	LUMINA_API extern SIZE_T GTotalRenderResourcesAllocated;
	
	constexpr uint64 GVersionSubmittedFlag = 0x8000000000000000;
	constexpr uint32 GVersionQueueShift = 60;
	constexpr uint32 GVersionQueueMask = 0x7;
	constexpr uint64 GVersionIDMask = 0x0FFFFFFFFFFFFFFF;

	constexpr uint64_t MakeVersion(uint64 ID, ECommandQueue Queue, bool Submitted)
	{
		uint64 Result = (ID & GVersionIDMask) | (uint64(Queue) << GVersionQueueShift);
		if (Submitted) Result |= GVersionSubmittedFlag;
		return Result;
	}

	constexpr uint64_t VersionGetInstance(uint64_t version)
	{
		return version & GVersionIDMask;
	}

	constexpr ECommandQueue VersionGetQueue(uint64_t version)
	{
		return ECommandQueue((version >> GVersionQueueShift) & GVersionQueueMask);
	}

	constexpr bool VersionGetSubmitted(uint64_t version)
	{
		return (version & GVersionSubmittedFlag) != 0;
	}

	struct FDrawIndirectArguments
	{
		uint32 VertexCount = 0;
		uint32 InstanceCount = 1;
		uint32 StartVertexLocation = 0;
		uint32 StartInstanceLocation = 0;

		constexpr FDrawIndirectArguments& SetVertexCount(uint32_t value) { VertexCount = value; return *this; }
		constexpr FDrawIndirectArguments& SetInstanceCount(uint32_t value) { InstanceCount = value; return *this; }
		constexpr FDrawIndirectArguments& SetStartVertexLocation(uint32_t value) { StartVertexLocation = value; return *this; }
		constexpr FDrawIndirectArguments& SetStartInstanceLocation(uint32_t value) { StartInstanceLocation = value; return *this; }
	};

	struct FDrawIndexedIndirectArguments
	{
		uint32 IndexCount = 0;
		uint32 InstanceCount = 1;
		uint32 StartIndexLocation = 0;
		int32  BaseVertexLocation = 0;
		uint32 StartInstanceLocation = 0;

		constexpr FDrawIndexedIndirectArguments& SetIndexCount(uint32 value) { IndexCount = value; return *this; }
		constexpr FDrawIndexedIndirectArguments& SetInstanceCount(uint32 value) { InstanceCount = value; return *this; }
		constexpr FDrawIndexedIndirectArguments& SetStartIndexLocation(uint32 value) { StartIndexLocation = value; return *this; }
		constexpr FDrawIndexedIndirectArguments& SetBaseVertexLocation(int32 value) { BaseVertexLocation = value; return *this; }
		constexpr FDrawIndexedIndirectArguments& SetStartInstanceLocation(uint32 value) { StartInstanceLocation = value; return *this; }
	};
	
	
    class LUMINA_API IRHIResource
    {
    public:

    	virtual ERHIResourceType GetResourceType() const = 0;

    	IRHIResource();
    	virtual ~IRHIResource();
    	IRHIResource(const IRHIResource&) = delete;
    	IRHIResource(const IRHIResource&&) = delete;
    	IRHIResource& operator=(const IRHIResource&) = delete;
    	IRHIResource& operator=(const IRHIResource&&) = delete;

    	static void ReleaseAllRHIResources();
    	
    	template<typename T, EAPIResourceType Type = EAPIResourceType::Default>
		T GetAPIResource()
    	{
    		void* Resource = GetAPIResourceImpl(Type);
    		return static_cast<T>(Resource);
    	}

    	void* GetAPIResource(EAPIResourceType Type)
    	{
    		void* Resource = GetAPIResourceImpl(Type);
    		return Resource;
    	}

    	int32 GetListIndex() const { return ListIndex; }
    	
    protected:

    	virtual void* GetAPIResourceImpl(EAPIResourceType Type) { return nullptr; }

    private:

    	void Destroy() const;
    	
    public:

    	        
    	uint32 AddRef() const
    	{
    		int32 NewValue = AtomicFlags.AddRef(std::memory_order_acquire);
    		Assert(NewValue > 0)
    		return uint32(NewValue);
    	}
        
    	uint32 Release()
    	{
    		int32 NewValue = AtomicFlags.Release(std::memory_order_release);
    		Assert(NewValue >= 0)

    		if (NewValue == 0)
    		{
    			Destroy();
    			return 0;
    		}
    		
    		return uint32(NewValue);
    	}
        
    	uint32 GetRefCount() const
    	{
    		int32 CurrentValue = AtomicFlags.GetNumRefs(std::memory_order_relaxed);
    		Assert(CurrentValue >= 0)
    		return uint32(CurrentValue);
    	}

    	bool IsValid() const
    	{
    		return AtomicFlags.IsValid(std::memory_order_relaxed);
    	}

    	void Delete()
    	{
    		Assert(!AtomicFlags.MarkForDelete(std::memory_order_acquire))
    		Memory::Delete(this);
    	}

    	NODISCARD bool Deleting() const
    	{
    		return AtomicFlags.Deleteing();
    	}
    	
    	//-----------------------------------------------------------------------------

        class FAtomicFlags
		{
			static constexpr uint32 MarkedForDeleteBit    = 1 << 30;
			static constexpr uint32 DeletingBit           = 1 << 31;
			static constexpr uint32 NumRefsMask           = ~(MarkedForDeleteBit | DeletingBit);
		
			std::atomic_uint Packed = { 0 };
		
		public:
    		
			int32 AddRef(std::memory_order MemoryOrder)
			{
				uint32 OldPacked = Packed.fetch_add(1, MemoryOrder);
				AssertMsg((OldPacked & DeletingBit) == 0, "Resource is being deleted.");
				int32  NumRefs = (OldPacked & NumRefsMask) + 1;
				AssertMsg(NumRefs < NumRefsMask, "Reference count has overflowed.");
				return NumRefs;
			}
		
			int32 Release(std::memory_order MemoryOrder)
			{
				uint32 OldPacked = Packed.fetch_sub(1, MemoryOrder);
				AssertMsg((OldPacked & DeletingBit) == 0, "Resource is being deleted.");
				int32  NumRefs = (OldPacked & NumRefsMask) - 1;
				AssertMsg(NumRefs >= 0, "Reference count has underflowed.");
				return NumRefs;
			}
		
			bool MarkForDelete(std::memory_order MemoryOrder)
			{
				uint32 OldPacked = Packed.fetch_or(MarkedForDeleteBit, MemoryOrder);
				Assert((OldPacked & DeletingBit) == 0);
				return (OldPacked & MarkedForDeleteBit) != 0;
			}
		
			bool UnmarkForDelete(std::memory_order MemoryOrder)
			{
				uint32 OldPacked = Packed.fetch_xor(MarkedForDeleteBit, MemoryOrder);
				Assert((OldPacked & DeletingBit) == 0);
				bool  OldMarkedForDelete = (OldPacked & MarkedForDeleteBit) != 0;
				Assert(OldMarkedForDelete == true);
				return OldMarkedForDelete;
			}
		
			bool Deleteing()
			{
				uint32 LocalPacked = Packed.load(std::memory_order_acquire);
				Assert((LocalPacked & MarkedForDeleteBit) != 0)
				Assert((LocalPacked & DeletingBit) == 0)
				uint32 NumRefs = LocalPacked & NumRefsMask;
		
				if (NumRefs == 0)
				{
					Packed.fetch_or(DeletingBit, std::memory_order_acquire);
					return true;
				}
				else
				{
					UnmarkForDelete(std::memory_order_release);
					return false;
				}
			}
		
			bool IsValid(std::memory_order MemoryOrder) const
			{
				uint32 LocalPacked = Packed.load(MemoryOrder);
				return (LocalPacked & MarkedForDeleteBit) == 0 && (LocalPacked & NumRefsMask) != 0;
			}
		
			bool IsMarkedForDelete(std::memory_order MemoryOrder) const
			{
				return (Packed.load(MemoryOrder) & MarkedForDeleteBit) != 0;
			}
		
			int32 GetNumRefs(std::memory_order MemoryOrder) const
			{
				return Packed.load(MemoryOrder) & NumRefsMask;
			}
		};
    
    private:
        
		mutable FAtomicFlags AtomicFlags;

		int32 ListIndex = INDEX_NONE;
    };

	//-------------------------------------------------------------------------------------------------------------------

	class IEventQuery : public IRHIResource { };
	

	class LUMINA_API FRHIViewport : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Viewport)
		
		FRHIViewport(const FIntVector2D& InSize, IRenderContext* InContext)
			: RenderContext(InContext)
			, Size(InSize)
		{
			CreateRenderTarget(InSize);
		}

		FRHIViewport(const FViewVolume& InVolume, const FIntVector2D& InSize, IRenderContext* InContext)
			: RenderContext(InContext)
			, ViewVolume(InVolume)
			, Size(InSize)
		{
			CreateRenderTarget(InSize);
		}
		
		void SetSize(const FIntVector2D& InSize);
		

		const FViewVolume& GetViewVolume() const { return ViewVolume; }
		const FIntVector2D& GetSize() const { return Size; }

		void SetViewVolume(const FViewVolume& InVolume) { ViewVolume = InVolume;}
		virtual FRHIImageRef GetRenderTarget() const { return RenderTarget; }

	private:

		void CreateRenderTarget(const FIntVector2D& InSize);

	protected:

		IRenderContext*		RenderContext;
		FRHIImageRef		RenderTarget;
		FViewVolume        	ViewVolume;
		FIntVector2D       	Size; 
    
	};


	//-------------------------------------------------------------------------------------------------------------------

	struct FDynamicBufferWrite
	{
		int64 LatestVersion = 0;
		int64 MinVersion = 0;
		int64 MaxVersion = 0;
		bool bInitialized = false;
	};

	struct LUMINA_API FRHIBufferDesc
	{
		uint64 Size = 0;
		uint32 Stride = 0;
		uint32 MaxVersions = 0;
		FString DebugName;
		EResourceStates InitialState = EResourceStates::Common;
		bool bKeepInitialState = false;
		TBitFlags<EBufferUsageFlags> Usage;

		FRHIBufferDesc() = default;
		FRHIBufferDesc(uint64 InSize, uint32 InStride, EBufferUsageFlags InUsage)
			: Size  (InSize)
			, Stride(InStride)
			, Usage (InUsage)
		{}

		static FRHIBufferDesc Null()
		{
			return {0, 0, BUF_NullResource};
		}

		NODISCARD bool IsNull() const
		{
			if (Usage.IsFlagSet(BUF_NullResource))
			{
				// The null resource descriptor should have its other fields zeroed, and no additional flags.
				Assert(Size == 0 && Stride == 0 && Usage.IsFlagSet(BUF_NullResource))
				return true;
			}

			return false;
		}
	};

	
	class LUMINA_API FRHIBuffer : public IRHIResource, public FBufferStateExtension
	{
	public:

		RENDER_RESOURCE(RRT_Buffer)

		FRHIBuffer(const FRHIBufferDesc& InDesc)
			: FBufferStateExtension(Description)
			, Description(InDesc)
		{}
		
		const FRHIBufferDesc& GetDescription() const { return Description; }

		bool IsStorageBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::StorageBuffer); }
		bool IsUniformBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::UniformBuffer); }
		bool IsVertexBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::VertexBuffer); }
		bool IsIndexBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::IndexBuffer); }

		
		uint64 GetSize() const { return Description.Size; }
		uint32 GetStride() const { return Description.Stride; }
		TBitFlags<EBufferUsageFlags> GetUsage() const { return Description.Usage; }

	protected:

		FRHIBufferDesc Description;
		
	};


	//-------------------------------------------------------------------------------------------------------------------
	
	
	/** 
	 * Descriptor used to define the properties of an image (texture) resource.
	 * This struct specifies how an image should be created, including its format,
	 * dimensions, and usage flags.
	 */
	struct FRHIImageDesc
	{
		/** Flags that define how the image will be used (e.g., render target, shader resource). */
		TBitFlags<EImageCreateFlags> Flags = 0;

		/** Additional data that may be used by specific platforms or backends. */
		uint32 ExtraData = 0;

		/** 
		 * The width and height of the image. 
		 * This is the primary size for 2D textures.
		 */
		FIntVector2D Extent = FIntVector2D(1);

		/** The depth of the image (used for 3D textures). */
		uint16 Depth = 1;

		/** The number of array layers (for texture arrays). */
		uint16 ArraySize = 1;

		/** The number of mip levels in the texture. */
		uint8 NumMips = 1;

		/** The number of samples for multi-sampled images (MSAA). */
		uint8 NumSamples = 1;

		/** The dimensionality of the image (1D, 2D, 3D, or Cube). */
		EImageDimension Dimension = EImageDimension::Texture2D;

		/** The format of the image (e.g., RGBA8, BC7 compressed, etc.). */
		EFormat Format = EFormat::UNKNOWN;

		/** Debug Name of this texture */
		FString DebugName;

		/** Initial state of this image. */
		EResourceStates InitialState = EResourceStates::Unknown;
		
		// If keepInitialState is true, command lists that use the texture will automatically
		// begin tracking the texture from the initial state and transition it to the initial state 
		// on command list close.
		bool bKeepInitialState = false;
		
		friend FArchive& operator << (FArchive& Ar, FRHIImageDesc& Data)
		{
			Ar << Data.Flags;
			Ar << Data.ExtraData;
			Ar << Data.Extent;
			Ar << Data.Depth;
			Ar << Data.ArraySize;
			Ar << Data.NumMips;
			Ar << Data.NumSamples;
			Ar << Data.Dimension;
			Ar << Data.Format;
			Ar << Data.DebugName;
			Ar << Data.InitialState;
			Ar << Data.bKeepInitialState;
			
			return Ar;
		}
		
	};

	// Describes a 2D or 3D section of a single mip level, single array slice of a texture.
	struct FTextureSlice
	{
		uint32 X = 0;
		uint32 Y = 0;
		uint32 Z = 0;

		uint32 MipLevel = 0;
		uint32 ArraySlice = 0;

		NODISCARD LUMINA_API FTextureSlice Resolve(const FRHIImageDesc& desc) const;

		constexpr FTextureSlice& SetOrigin(uint32 vx = 0, uint32 vy = 0, uint32 vz = 0) { X = vx; Y = vy; Z = vz; return *this; }
		constexpr FTextureSlice& SetMipLevel(uint32 level) { MipLevel = level; return *this; }
		constexpr FTextureSlice& SetArraySlice(uint32 slice) { ArraySlice = slice; return *this; }
	};

	struct FTextureSubresourceSet
    {
        static constexpr uint32 AllMipLevels = uint32(-1);
        static constexpr uint32 AllArraySlices = uint32(-1);
        
        uint32 BaseMipLevel = 0;
        uint32 NumMipLevels = 1;
        uint32 BaseArraySlice = 0;
        uint32 NumArraySlices = 1;

        FTextureSubresourceSet() = default;

        FTextureSubresourceSet(uint32 _baseMipLevel, uint32 _numMipLevels, uint32 _baseArraySlice, uint32 _numArraySlices)
            : BaseMipLevel(_baseMipLevel)
            , NumMipLevels(_numMipLevels)
            , BaseArraySlice(_baseArraySlice)
            , NumArraySlices(_numArraySlices)
        {}

        NODISCARD LUMINA_API FTextureSubresourceSet Resolve(const FRHIImageDesc& Desc, bool bSingleMipLevel) const;
        NODISCARD LUMINA_API bool IsEntireTexture(const FRHIImageDesc& Desc) const;

        bool operator ==(const FTextureSubresourceSet& other) const
        {
            return BaseMipLevel == other.BaseMipLevel &&
                NumMipLevels == other.NumMipLevels &&
                BaseArraySlice == other.BaseArraySlice &&
                NumArraySlices == other.NumArraySlices;
        }
        bool operator !=(const FTextureSubresourceSet& other) const { return !(*this == other); }

        constexpr FTextureSubresourceSet& SetBaseMipLevel(uint32 value) { BaseMipLevel = value; return *this; }
        constexpr FTextureSubresourceSet& SetNumMipLevels(uint32 value) { NumMipLevels = value; return *this; }
        constexpr FTextureSubresourceSet& SetMipLevels(uint32 base, uint32 num) { BaseMipLevel = base; NumMipLevels = num; return *this; }
        constexpr FTextureSubresourceSet& SetBaseArraySlice(uint32 value) { BaseArraySlice = value; return *this; }
        constexpr FTextureSubresourceSet& SetNumArraySlices(uint32 value) { NumArraySlices = value; return *this; }
        constexpr FTextureSubresourceSet& SetArraySlices(uint32 base, uint32 num) { BaseArraySlice = base; NumArraySlices = num; return *this; }

        // see the bottom of this file for a specialization of std::hash<TextureSubresourceSet>
    };

    static const FTextureSubresourceSet AllSubresources = FTextureSubresourceSet(0, FTextureSubresourceSet::AllMipLevels, 0, FTextureSubresourceSet::AllArraySlices);
	
	enum class ESamplerAddressMode : uint8
	{
		// D3D names
		Clamp = 0,
		Wrap = 1,
		Border = 2,
		Mirror = 3,
		MirrorOnce = 4,

		// Vulkan names
		ClampToEdge = Clamp,
		Repeat = Wrap,
		ClampToBorder = Border,
		MirroredRepeat = Mirror,
		MirrorClampToEdge = MirrorOnce
	};

#define AM_Clamp ESamplerAddressMode::Clamp
#define AM_Wrap ESamplerAddressMode::Wrap
#define AM_Repeat ESamplerAddressMode::Repeat
#define AM_Border ESamplerAddressMode::Border
#define AM_Mirror ESamplerAddressMode::Mirror
#define AM_MirrorOnce ESamplerAddressMode::MirrorOnce

	enum class LUMINA_API ESamplerReductionType : uint8_t
	{
		Standard,
		Comparison,
		Minimum,
		Max
	};

	struct LUMINA_API FSamplerDesc
	{
		FColor BorderColor = 1.0f;
		float MaxAnisotropy = 1.0f;
		float MipBias = 0.0f;

		bool MinFilter = true;
		bool MagFilter = true;
		bool MipFilter = true;
		ESamplerAddressMode AddressU = ESamplerAddressMode::Clamp;
		ESamplerAddressMode AddressV = ESamplerAddressMode::Clamp;
		ESamplerAddressMode AddressW = ESamplerAddressMode::Clamp;
		ESamplerReductionType ReductionType = ESamplerReductionType::Standard;

		FSamplerDesc& SetBorderColor(const FColor& color) { BorderColor = color; return *this; }
		FSamplerDesc& SetMaxAnisotropy(float value) { MaxAnisotropy = value; return *this; }
		FSamplerDesc& SetMipBias(float value) { MipBias = value; return *this; }
		FSamplerDesc& SetMinFilter(bool enable) { MinFilter = enable; return *this; }
		FSamplerDesc& SetMagFilter(bool enable) { MagFilter = enable; return *this; }
		FSamplerDesc& SetMipFilter(bool enable) { MipFilter = enable; return *this; }
		FSamplerDesc& SetAllFilters(bool enable) { MinFilter = MagFilter = MipFilter = enable; return *this; }
		FSamplerDesc& SetAddressU(ESamplerAddressMode mode) { AddressU = mode; return *this; }
		FSamplerDesc& SetAddressV(ESamplerAddressMode mode) { AddressV = mode; return *this; }
		FSamplerDesc& SetAddressW(ESamplerAddressMode mode) { AddressW = mode; return *this; }
		FSamplerDesc& SetAllAddressModes(ESamplerAddressMode mode) { AddressU = AddressV = AddressW = mode; return *this; }
		FSamplerDesc& SetReductionType(ESamplerReductionType type) { ReductionType = type; return *this; }
	};

	class LUMINA_API FRHISampler : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_SamplerState)

		NODISCARD virtual const FSamplerDesc& GetDesc() const = 0;

		
	};
	
	class LUMINA_API FRHIImage : public IRHIResource, public FTextureStateExtension
	{
	public:

		RENDER_RESOURCE(RRT_Image)

		FRHIImage(const FRHIImageDesc& InDesc)
			: FTextureStateExtension(Description)
			, Description(InDesc)
		{}
		
		FORCEINLINE const FRHIImageDesc& GetDescription() const { return Description; }
		
		FORCEINLINE const FIntVector2D& GetExtent() const { return Description.Extent; }
		FORCEINLINE uint32 GetSizeX() const { return Description.Extent.X; }
		FORCEINLINE uint32 GetSizeY() const { return Description.Extent.Y; }
		FORCEINLINE EFormat GetFormat() const { return Description.Format; }
		FORCEINLINE TBitFlags<EImageCreateFlags> GetFlags() const { return Description.Flags; }
		
	
	private:

		FRHIImageDesc Description;
	};

	//-------------------------------------------------------------------------------------------------------------------

	class LUMINA_API FRHIShader : public IRHIResource
	{
	public:

		FName GetKey() const { return Key; }
		void SetKey(const FName& InKey) { Key = InKey; }
		
		/** Get the shader's native representation of it's bytecode */
		virtual void GetByteCode(const void** ByteCode, uint64* Size) = 0;
	
	private:
		
		FName Key;
	};

	class LUMINA_API FRHIVertexShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_VertexShader)

	};

	class LUMINA_API FRHIPixelShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_PixelShader)

	};
	
	class LUMINA_API FRHIComputeShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_ComputeShader)

	};

	class LUMINA_API FShaderLibrary : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_ShaderLibrary)

		void AddShader(FRHIShader* Shader);
		void RemoveShader(FName Key);

		template<typename T>
		TRefCountPtr<T> GetShader(const FName& Key)
		{
			return GetShader(Key).As<T>();
		}

		FRHIShaderRef GetShader(const FName& Key);

	private:

		FMutex Mutex;
		THashMap<FName, FRHIShaderRef> Shaders;
    
	};
	
	//-------------------------------------------------------------------------------------------------------------------

	enum class LUMINA_API ERasterFillMode : uint8_t
	{
		Solid,
		Wireframe,

		// Vulkan names
		Fill = Solid,
		Line = Wireframe
	};

	enum class LUMINA_API ERasterCullMode : uint8
	{
		Back,
		Front,
		None
	};

	enum class LUMINA_API EBlendFactor : uint8
	{
		Zero = 1,
		One = 2,
		SrcColor = 3,
		InvSrcColor = 4,
		SrcAlpha = 5,
		InvSrcAlpha = 6,
		DstAlpha  = 7,
		InvDstAlpha = 8,
		DstColor = 9,
		InvDstColor = 10,
		SrcAlphaSaturate = 11,
		ConstantColor = 14,
		InvConstantColor = 15,
		Src1Color = 16,
		InvSrc1Color = 17,
		Src1Alpha = 18,
		InvSrc1Alpha = 19,

		// Vulkan names
		OneMinusSrcColor = InvSrcColor,
		OneMinusSrcAlpha = InvSrcAlpha,
		OneMinusDstAlpha = InvDstAlpha,
		OneMinusDstColor = InvDstColor,
		OneMinusConstantColor = InvConstantColor,
		OneMinusSrc1Color = InvSrc1Color,
		OneMinusSrc1Alpha = InvSrc1Alpha,
	};
    
	enum class LUMINA_API EBlendOp : uint8
	{
		Add = 1,
		Subrtact = 2,
		ReverseSubtract = 3,
		Min = 4,
		Max = 5
	};

	enum class LUMINA_API EColorMask : uint8
	{
		// These values are equal to their counterparts in DX11, DX12, and Vulkan.
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 0xF
	};

	ENUM_CLASS_FLAGS(EColorMask)

	struct LUMINA_API FVertexAttributeDesc
	{
		EFormat Format = EFormat::UNKNOWN;
		uint32 ArraySize = 1;
		uint32 BufferIndex = 0;
		uint32 Offset = 0;
		uint32 ElementStride = 0;
		bool bInstanced = false;
		
		constexpr FVertexAttributeDesc& SetArraySize(uint32 value) { ArraySize = value; return *this; }
		constexpr FVertexAttributeDesc& SetBufferIndex(uint32 value) { BufferIndex = value; return *this; }
		constexpr FVertexAttributeDesc& SetOffset(uint32 value) { Offset = value; return *this; }
		constexpr FVertexAttributeDesc& SetElementStride(uint32 value) { ElementStride = value; return *this; }
		constexpr FVertexAttributeDesc& SetIsInstanced(bool value) { bInstanced = value; return *this; }
	};
	
	class LUMINA_API IRHIInputLayout : public IRHIResource
	{
	public:
		
		NODISCARD virtual uint32 GetNumAttributes() const = 0;
		NODISCARD virtual const FVertexAttributeDesc* GetAttributeDesc(uint32 index) const = 0;
	};
	
	struct LUMINA_API FBlendState
    {
        struct RenderTarget
        {
            bool			bBlendEnable = false;
            EBlendFactor 	SrcBlend = EBlendFactor::One;
            EBlendFactor 	DestBlend = EBlendFactor::Zero;
            EBlendOp     	BlendOp = EBlendOp::Add;
            EBlendFactor 	SrcBlendAlpha = EBlendFactor::One;
            EBlendFactor 	DestBlendAlpha = EBlendFactor::Zero;
            EBlendOp     	BlendOpAlpha = EBlendOp::Add;
            EColorMask   	ColorWriteMask = EColorMask::All;
        	EFormat			Format = EFormat::BGRA8_UNORM;
        	bool			bEnabled = false;

            constexpr RenderTarget& SetBlendEnable(bool enable) { bBlendEnable = enable; return *this; }
            constexpr RenderTarget& EnableBlend() { bBlendEnable = true; return *this; }
            constexpr RenderTarget& DisableBlend() { bBlendEnable = false; return *this; }
            constexpr RenderTarget& SetSrcBlend(EBlendFactor value) { SrcBlend = value; return *this; }
            constexpr RenderTarget& SetDestBlend(EBlendFactor value) { DestBlend = value; return *this; }
            constexpr RenderTarget& SetBlendOp(EBlendOp value) { BlendOp = value; return *this; }
            constexpr RenderTarget& SetSrcBlendAlpha(EBlendFactor value) { SrcBlendAlpha = value; return *this; }
            constexpr RenderTarget& SetDestBlendAlpha(EBlendFactor value) { DestBlendAlpha = value; return *this; }
            constexpr RenderTarget& SetBlendOpAlpha(EBlendOp value) { BlendOpAlpha = value; return *this; }
            constexpr RenderTarget& SetColorWriteMask(EColorMask value) { ColorWriteMask = value; return *this; }
        	constexpr RenderTarget& SetFormat(EFormat InFormat) { Format = InFormat; return *this; }
        	
            constexpr bool operator ==(const RenderTarget& other) const
            {
                return bBlendEnable == other.bBlendEnable
                    && SrcBlend == other.SrcBlend
                    && DestBlend == other.DestBlend
                    && BlendOp == other.BlendOp
                    && SrcBlendAlpha == other.SrcBlendAlpha
                    && DestBlendAlpha == other.DestBlendAlpha
                    && BlendOpAlpha == other.BlendOpAlpha
                    && ColorWriteMask == other.ColorWriteMask
					&& Format == other.Format;
            }

            constexpr bool operator !=(const RenderTarget& other) const
            {
                return !(*this == other);
            }
        };

        RenderTarget Targets[MaxRenderTargets];
        bool AlphaToCoverageEnable = false;

        constexpr FBlendState& SetRenderTarget(uint32 index, const RenderTarget& target) { Targets[index] = target; Targets[index].bEnabled = true; return *this; }
        constexpr FBlendState& SetAlphaToCoverageEnable(bool enable) { AlphaToCoverageEnable = enable; return *this; }
        constexpr FBlendState& EnableAlphaToCoverage() { AlphaToCoverageEnable = true; return *this; }
        constexpr FBlendState& DisableAlphaToCoverage() { AlphaToCoverageEnable = false; return *this; }
		
        constexpr bool operator ==(const FBlendState& other) const
        {
            if (AlphaToCoverageEnable != other.AlphaToCoverageEnable)
                return false;

            for (uint32_t i = 0; i < MaxRenderTargets; ++i)
            {
                if (Targets[i] != other.Targets[i])
                {
	                return false;
                }
            }

            return true;
        }

        constexpr bool operator !=(const FBlendState& other) const
        {
            return !(*this == other);
        }
    };

	
	//-------------------------------------------------------------------------------------------------------------------

	
	struct LUMINA_API FRasterState
    {
        ERasterFillMode FillMode = ERasterFillMode::Solid;
        ERasterCullMode CullMode = ERasterCullMode::Back;
        bool FrontCounterClockwise = true;
        bool DepthClipEnable = false;
        bool ScissorEnable = false;
        bool MultisampleEnable = false;
        bool AntialiasedLineEnable = false;
        int DepthBias = 0;
        float DepthBiasClamp = 0.0f;
        float SlopeScaledDepthBias = 0.0f;


        uint8 ForcedSampleCount = 0;
        bool ProgrammableSamplePositionsEnable = false;
        bool ConservativeRasterEnable = false;
        bool QuadFillEnable = false;
        char SamplePositionsX[16]{};
        char SamplePositionsY[16]{};
        
        constexpr FRasterState& SetFillMode(ERasterFillMode value) { FillMode = value; return *this; }
        constexpr FRasterState& SetFillSolid() { FillMode = ERasterFillMode::Solid; return *this; }
        constexpr FRasterState& SetFillWireframe() { FillMode = ERasterFillMode::Wireframe; return *this; }
        constexpr FRasterState& SetCullMode(ERasterCullMode value) { CullMode = value; return *this; }
        constexpr FRasterState& SetCullBack() { CullMode = ERasterCullMode::Back; return *this; }
        constexpr FRasterState& SetCullFront() { CullMode = ERasterCullMode::Front; return *this; }
        constexpr FRasterState& SetCullNone() { CullMode = ERasterCullMode::None; return *this; }
        constexpr FRasterState& SetFrontCounterClockwise(bool value) { FrontCounterClockwise = value; return *this; }
        constexpr FRasterState& SetDepthClipEnable(bool value) { DepthClipEnable = value; return *this; }
        constexpr FRasterState& EnableDepthClip() { DepthClipEnable = true; return *this; }
        constexpr FRasterState& DisableDepthClip() { DepthClipEnable = false; return *this; }
        constexpr FRasterState& SetScissorEnable(bool value) { ScissorEnable = value; return *this; }
        constexpr FRasterState& EnableScissor() { ScissorEnable = true; return *this; }
        constexpr FRasterState& DisableScissor() { ScissorEnable = false; return *this; }
        constexpr FRasterState& SetMultisampleEnable(bool value) { MultisampleEnable = value; return *this; }
        constexpr FRasterState& EnableMultisample() { MultisampleEnable = true; return *this; }
        constexpr FRasterState& DisableMultisample() { MultisampleEnable = false; return *this; }
        constexpr FRasterState& SetAntialiasedLineEnable(bool value) { AntialiasedLineEnable = value; return *this; }
        constexpr FRasterState& EnableAntialiasedLine() { AntialiasedLineEnable = true; return *this; }
        constexpr FRasterState& DisableAntialiasedLine() { AntialiasedLineEnable = false; return *this; }
        constexpr FRasterState& SetDepthBias(int value) { DepthBias = value; return *this; }
        constexpr FRasterState& SetDepthBiasClamp(float value) { DepthBiasClamp = value; return *this; }
        constexpr FRasterState& SetSlopeScaleDepthBias(float value) { SlopeScaledDepthBias = value; return *this; }
        constexpr FRasterState& SetForcedSampleCount(uint8_t value) { ForcedSampleCount = value; return *this; }
        constexpr FRasterState& SetProgrammableSamplePositionsEnable(bool value) { ProgrammableSamplePositionsEnable = value; return *this; }
        constexpr FRasterState& EnableProgrammableSamplePositions() { ProgrammableSamplePositionsEnable = true; return *this; }
        constexpr FRasterState& DisableProgrammableSamplePositions() { ProgrammableSamplePositionsEnable = false; return *this; }
        constexpr FRasterState& SetConservativeRasterEnable(bool value) { ConservativeRasterEnable = value; return *this; }
        constexpr FRasterState& EnableConservativeRaster() { ConservativeRasterEnable = true; return *this; }
        constexpr FRasterState& DisableConservativeRaster() { ConservativeRasterEnable = false; return *this; }
        constexpr FRasterState& SetQuadFillEnable(bool value) { QuadFillEnable = value; return *this; }
        constexpr FRasterState& EnableQuadFill() { QuadFillEnable = true; return *this; }
        constexpr FRasterState& DisableQuadFill() { QuadFillEnable = false; return *this; }
        constexpr FRasterState& SetSamplePositions(const char* x, const char* y, int count) { for (int i = 0; i < count; i++) { SamplePositionsX[i] = x[i]; SamplePositionsY[i] = y[i]; } return *this; }
    };

	enum class LUMINA_API EStencilOp : uint8
	{
		Keep = 1,
		Zero = 2,
		Replace = 3,
		IncrementAndClamp = 4,
		DecrementAndClamp = 5,
		Invert = 6,
		IncrementAndWrap = 7,
		DecrementAndWrap = 8
	};

	enum class LUMINA_API EComparisonFunc : uint8
	{
		Never = 1,
		Less = 2,
		Equal = 3,
		LessOrEqual = 4,
		Greater = 5,
		NotEqual = 6,
		GreaterOrEqual = 7,
		Always = 8
	};
	
	struct LUMINA_API FDepthStencilState
    {
        struct StencilOpDesc
        {
            EStencilOp FailOp = EStencilOp::Keep;
            EStencilOp DepthFailOp = EStencilOp::Keep;
            EStencilOp PassOp = EStencilOp::Keep;
            EComparisonFunc StencilFunc = EComparisonFunc::Always;

            constexpr StencilOpDesc& setFailOp(EStencilOp value) { FailOp = value; return *this; }
            constexpr StencilOpDesc& setDepthFailOp(EStencilOp value) { DepthFailOp = value; return *this; }
            constexpr StencilOpDesc& setPassOp(EStencilOp value) { PassOp = value; return *this; }
            constexpr StencilOpDesc& setStencilFunc(EComparisonFunc value) { StencilFunc = value; return *this; }

			const size_t GetHash() const
            {
	            size_t hash = 0;
            	Hash::HashCombine(hash, FailOp);
            	Hash::HashCombine(hash, DepthFailOp);
            	Hash::HashCombine(hash, PassOp);
            	Hash::HashCombine(hash, StencilFunc);
            	return hash;
            }
        };

        bool            	DepthTestEnable = true;
        bool            	DepthWriteEnable = true;
        EComparisonFunc		DepthFunc = EComparisonFunc::Less;
        bool            	StencilEnable = false;
        uint8         		StencilReadMask = 0xff;
        uint8         		StencilWriteMask = 0xff;
        uint8         		StencilRefValue = 0;
        bool            	DynamicStencilRef = false;
        StencilOpDesc   	FrontFaceStencil;
        StencilOpDesc   	BackFaceStencil;

        constexpr FDepthStencilState& SetDepthTestEnable(bool value) { DepthTestEnable = value; return *this; }
        constexpr FDepthStencilState& EnableDepthTest() { DepthTestEnable = true; return *this; }
        constexpr FDepthStencilState& DisableDepthTest() { DepthTestEnable = false; return *this; }
        constexpr FDepthStencilState& SetDepthWriteEnable(bool value) { DepthWriteEnable = value; return *this; }
        constexpr FDepthStencilState& EnableDepthWrite() { DepthWriteEnable = true; return *this; }
        constexpr FDepthStencilState& DisableDepthWrite() { DepthWriteEnable = false; return *this; }
        constexpr FDepthStencilState& SetDepthFunc(EComparisonFunc value) { DepthFunc = value; return *this; }
        constexpr FDepthStencilState& SetStencilEnable(bool value) { StencilEnable = value; return *this; }
        constexpr FDepthStencilState& EnableStencil() { StencilEnable = true; return *this; }
        constexpr FDepthStencilState& DisableStencil() { StencilEnable = false; return *this; }
        constexpr FDepthStencilState& SetStencilReadMask(uint8 value) { StencilReadMask = value; return *this; }
        constexpr FDepthStencilState& SetStencilWriteMask(uint8 value) { StencilWriteMask = value; return *this; }
        constexpr FDepthStencilState& SetStencilRefValue(uint8 value) { StencilRefValue = value; return *this; }
        constexpr FDepthStencilState& SetFrontFaceStencil(const StencilOpDesc& value) { FrontFaceStencil = value; return *this; }
        constexpr FDepthStencilState& SetBackFaceStencil(const StencilOpDesc& value) { BackFaceStencil = value; return *this; }
        constexpr FDepthStencilState& SetDynamicStencilRef(bool value) { DynamicStencilRef = value; return *this; }
        
    };

	struct FBufferRange
	{
		uint64 ByteOffset = 0;
		uint64 ByteSize = 0;
        
		FBufferRange() = default;

		FBufferRange(uint64 _byteOffset, uint64 _byteSize)
			: ByteOffset(_byteOffset)
			, ByteSize(_byteSize)
		{ }

		NODISCARD LUMINA_API FBufferRange Resolve(const FRHIBufferDesc& Desc) const;
		NODISCARD constexpr bool IsEntireBuffer(const FRHIBufferDesc& desc) const { return (ByteOffset == 0) && (ByteSize == ~0ull || ByteSize == desc.Size); }
		constexpr bool operator == (const FBufferRange& other) const { return ByteOffset == other.ByteOffset && ByteSize == other.ByteSize; }

		constexpr FBufferRange& SetByteOffset(uint64 value) { ByteOffset = value; return *this; }
		constexpr FBufferRange& SetByteSize(uint64 value) { ByteSize = value; return *this; }
	};

	static const FBufferRange EntireBuffer = FBufferRange(0, ~0ull);


	struct LUMINA_API FSinglePassStereoState
	{
		bool bEnabled = false;
		bool bIndependentViewportMask = false;
		uint16 RenderTargetIndexOffset = 0;

		bool operator ==(const FSinglePassStereoState& b) const
		{
			return bEnabled == b.bEnabled
				&& bIndependentViewportMask == b.bIndependentViewportMask
				&& RenderTargetIndexOffset == b.RenderTargetIndexOffset;
		}

		bool operator !=(const FSinglePassStereoState& b) const { return !(*this == b); }

		constexpr FSinglePassStereoState& SetEnabled(bool value) { bEnabled = value; return *this; }
		constexpr FSinglePassStereoState& SetIndependentViewportMask(bool value) { bIndependentViewportMask = value; return *this; }
		constexpr FSinglePassStereoState& SetRenderTargetIndexOffset(uint16 value) { RenderTargetIndexOffset = value; return *this; }
	};
	
	struct LUMINA_API FRenderState
	{
		FBlendState				BlendState;
		FDepthStencilState		DepthStencilState;
		FRasterState			RasterState;
		FSinglePassStereoState	SinglePassStereo;

		constexpr FRenderState& SetBlendState(const FBlendState& value) { BlendState = value; return *this; }
		constexpr FRenderState& SetDepthStencilState(const FDepthStencilState& value) { DepthStencilState = value; return *this; }
		constexpr FRenderState& SetRasterState(const FRasterState& value) { RasterState = value; return *this; }
		constexpr FRenderState& SetSinglePassStereoState(const FSinglePassStereoState& value) { SinglePassStereo = value; return *this; }
	};

	struct LUMINA_API FBindingLayoutItem
	{
		uint32 Slot;
		
		ERHIBindingResourceType Type	: 8;
		uint16 Size						: 16;

		static FBindingLayoutItem Buffer_UD(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Buffer_Uniform_Dynamic;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem Buffer_SD(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Buffer_Storage_Dynamic;
			Result.Size = Size;
			return Result;
		}
		
		static FBindingLayoutItem Buffer_SRV(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Buffer_SRV;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem Buffer_UAV(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Buffer_UAV;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem Buffer_CBV(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Buffer_CBV;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem Texture_SRV(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Texture_SRV;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem Texture_UAV(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::Texture_UAV;
			Result.Size = Size;
			return Result;
		}

		static FBindingLayoutItem PushConstants(uint32 Slot, uint16 Size = 0)
		{
			FBindingLayoutItem Result;
			Result.Slot = Slot;
			Result.Type = ERHIBindingResourceType::PushConstants;
			Result.Size = Size;
			return Result;
		}
		
		bool operator ==(const FBindingLayoutItem& b) const
		{
			return Slot == b.Slot
				&& Type == b.Type
				&& Size == b.Size;
		}
	};
	
	
	struct LUMINA_API FBindingLayoutDesc
	{
		TBitFlags<ERHIShaderType> StageFlags;
		uint32 Index = 0;

		TFixedVector<FBindingLayoutItem, MaxBindingsPerLayout> Bindings;

		
		FBindingLayoutDesc& SetVisibility(ERHIShaderType InType) { StageFlags.SetFlag(InType); return *this; }
		FBindingLayoutDesc& SetBindingIndex(uint32 Value) { Index = Value; return *this; }
		FBindingLayoutDesc& AddItem(const FBindingLayoutItem& Item) { Bindings.push_back(Item); return *this; }
	};

	struct LUMINA_API FBindlessLayoutDesc
	{
		TBitFlags<ERHIShaderType> StageFlags;
		uint32 FirstSlot = 0;
		uint32 MaxCapacity = 0;
		TFixedVector<FBindingLayoutItem, MaxBindingsPerLayout> Bindings;

		FBindlessLayoutDesc& SetVisibility(ERHIShaderType Value) { StageFlags.SetFlag(Value); return *this; }
		FBindlessLayoutDesc& SetFirstSlot(uint32 Value) { FirstSlot = Value; return *this; }
		FBindlessLayoutDesc& SetMaxCapacity(uint32 Value) { MaxCapacity = Value; return *this; }
		FBindlessLayoutDesc& AddBinding(const FBindingLayoutItem& value) { Bindings.push_back(value); return *this; }
		

	};

	struct LUMINA_API FBindingTextureResource
	{
		FTextureSubresourceSet	Subresources;
		FRHISampler*			Sampler;
	};
	
	struct LUMINA_API FBindingSetItem
	{
		FBindingSetItem() { }
		
		IRHIResource* ResourceHandle;
		uint32 Slot;
		uint32 ArrayElement;
		ERHIBindingResourceType Type	: 8;
		EImageDimension Dimension		: 8;
		EFormat Format					: 8;
		uint8 bUnused					: 8;
		uint32 bUnused2; // Padding

		union 
		{
			FBindingTextureResource TextureResource;	// Valid for Texture_SRV, Texture_UAV
			FBufferRange Range;							// Valid for Buffer_SRV, Buffer_UAV, ConstantBuffer
			uint64 RawData[2];
		};

		// verify that the `subresources` and `range` have the same size and are covered by `rawData`
		static_assert(sizeof(FTextureSubresourceSet) == 16, "sizeof(TextureSubresourceSet) is supposed to be 16 bytes");
		static_assert(sizeof(FBufferRange) == 16, "sizeof(BufferRange) is supposed to be 16 bytes");
		
		// Creates a Shader Resource View (SRV) binding for a buffer.
		// Typically used for read-only access to structured or byte-address buffers.
		static FBindingSetItem BufferSRV(uint32 Slot, FRHIBuffer* Buffer, EFormat Format = EFormat::UNKNOWN, FBufferRange Range = EntireBuffer)
		{
			bool bIsDynamic = Buffer->GetDescription().Usage.IsFlagSet(BUF_Dynamic);
			FBindingSetItem Result;
			Result.Type = bIsDynamic ? ERHIBindingResourceType::Buffer_Storage_Dynamic : ERHIBindingResourceType::Buffer_SRV;
			Result.ResourceHandle = Buffer;
			Result.Range = Range;
			Result.Dimension = EImageDimension::Unknown;
			Result.TextureResource.Sampler = nullptr;
			Result.Format = Format;
			Result.ArrayElement = 0;
			Result.Slot = Slot;
			Result.RawData[0] = 0;
			Result.RawData[1] = 0;
			Result.bUnused = 0;
			Result.bUnused2 = 0;
			
			return Result;
		}

		// Creates an Unordered Access View (UAV) binding for a buffer.
		// Used for read-write access in shaders (e.g. compute shaders).
		static FBindingSetItem BufferUAV(uint32 Slot, FRHIBuffer* Buffer, EFormat Format = EFormat::UNKNOWN, FBufferRange Range = EntireBuffer)
		{
			bool bIsDynamic = Buffer->GetDescription().Usage.IsFlagSet(BUF_Dynamic);
			FBindingSetItem Result;
			Result.Type = bIsDynamic ? ERHIBindingResourceType::Buffer_Storage_Dynamic : ERHIBindingResourceType::Buffer_UAV;
			Result.ResourceHandle = Buffer;
			Result.Range = Range;
			Result.Dimension = EImageDimension::Unknown;
			Result.TextureResource.Sampler = nullptr;
			Result.Format = Format;
			Result.ArrayElement = 0;
			Result.Slot = Slot;
			Result.RawData[0] = 0;
			Result.RawData[1] = 0;
			Result.bUnused = 0;
			Result.bUnused2 = 0;
			
			return Result;
		}

		// Creates a Constant Buffer View (CBV) binding for a buffer.
		// Used to bind uniform buffers (read-only, small constant data).
		static FBindingSetItem BufferCBV(uint32 Slot, FRHIBuffer* Buffer, FBufferRange Range = EntireBuffer)
		{
			bool bIsDynamic = Buffer->GetDescription().Usage.IsFlagSet(BUF_Dynamic);
			FBindingSetItem Result;
			Result.Type = bIsDynamic ? ERHIBindingResourceType::Buffer_Uniform_Dynamic : ERHIBindingResourceType::Buffer_CBV;
			Result.ResourceHandle = Buffer;
			Result.Range = Range;
			Result.Dimension = EImageDimension::Unknown;
			Result.Format = EFormat::UNKNOWN;
			Result.TextureResource.Sampler = nullptr;
			Result.ArrayElement = 0;
			Result.Slot = Slot;
			Result.RawData[0] = 0;
			Result.RawData[1] = 0;
			Result.bUnused = 0;
			Result.bUnused2 = 0;
			
			return Result;
		}

		// Creates a Shader Resource View (SRV) binding for a texture/image.
		// Used to read from a texture in shaders (e.g. sampling or texel fetch).
		static FBindingSetItem TextureSRV(uint32 Slot, FRHIImage* Image, FRHISampler* Sampler = nullptr, EFormat Format = EFormat::UNKNOWN, FTextureSubresourceSet Subresources = AllSubresources, EImageDimension Dimension = EImageDimension::Unknown)
		{
			FBindingSetItem Result;
			Result.Slot = Slot;
			Result.ArrayElement = 0;
			Result.Type = ERHIBindingResourceType::Texture_SRV;
			Result.ResourceHandle = Image;
			Result.Format = Format;
			Result.Dimension = Dimension;
			Result.TextureResource.Subresources = Subresources;
			Result.TextureResource.Sampler = Sampler;
			Result.bUnused = 0;
			Result.bUnused2 = 0;
			
			return Result;
		}

		
		// Creates an Unordered Access View (UAV) binding for a texture/image.
		// Used for read-write access to a texture (e.g. compute shader writes).
		static FBindingSetItem TextureUAV(uint32 Slot, FRHIImage* Image, EFormat Format = EFormat::UNKNOWN, FTextureSubresourceSet Subresources =
			FTextureSubresourceSet(0, 1, 0, FTextureSubresourceSet::AllArraySlices),
			EImageDimension Dimension = EImageDimension::Unknown)
		{
			FBindingSetItem Result;
			Result.Slot = Slot;
			Result.ArrayElement = 0;
			Result.Type = ERHIBindingResourceType::Texture_UAV;
			Result.ResourceHandle = Image;
			Result.Format = Format;
			Result.Dimension = Dimension;
			Result.TextureResource.Subresources = Subresources;
			Result.TextureResource.Sampler = nullptr;
			Result.bUnused = 0;
			Result.bUnused2 = 0;
			
			return Result;
		}

		bool operator == (const FBindingSetItem& b) const
		{
			return ResourceHandle == b.ResourceHandle
				&& Slot == b.Slot
				&& Type == b.Type
				&& RawData[0] == b.RawData[0]
				&& RawData[1] == b.RawData[1];
		}

	};

	// Verify the packing of BindingSetItem for good alignment.
	static_assert(sizeof(FBindingSetItem) == 48, "sizeof(FBindingSetItem) is supposed to be 48 bytes");
	
	struct LUMINA_API FBindingSetDesc
	{
		TFixedVector<FBindingSetItem, MaxBindingsPerLayout> Bindings;
		
		bool operator ==(const FBindingSetDesc& b) const
		{
			if (Bindings.size() != b.Bindings.size())
				return false;

			for (size_t i = 0; i < Bindings.size(); ++i)
			{
				if (Bindings[i] != b.Bindings[i])
				{
					return false;
				}
			}

			return true;
		}

		bool operator !=(const FBindingSetDesc& b) const
		{
			return !(*this == b);
		}

		FBindingSetDesc& AddItem(const FBindingSetItem& value) { Bindings.push_back(value); return *this; }
	};

	
	class LUMINA_API FRHIBindingLayout : public IRHIResource
	{
	public:
		
		NODISCARD virtual const FBindingLayoutDesc* GetDesc() const = 0;
		NODISCARD virtual const FBindlessLayoutDesc* GetBindlessDesc() const = 0;

	};

	class LUMINA_API FRHIBindingSet : public IRHIResource
	{
	public:
		
		NODISCARD virtual const FBindingSetDesc* GetDesc() const = 0;
		NODISCARD virtual FRHIBindingLayout* GetLayout() const = 0;
		
	};
	
	class LUMINA_API FRHIDescriptorTable : public FRHIBindingSet
	{
	public:
		
		NODISCARD virtual uint32_t GetCapacity() const = 0;
		NODISCARD virtual uint32_t GetFirstDescriptorIndexInHeap() const = 0;
	};

	enum class LUMINA_API EVariableShadingRate : uint8
	{
		e1x1,
		e1x2,
		e2x1,
		e2x2,
		e2x4,
		e4x2,
		e4x4
	};

	enum class LUMINA_API EShadingRateCombiner : uint8
	{
		Passthrough,
		Override,
		Min,
		Max,
		ApplyRelative
	};

	struct LUMINA_API FVariableRateShadingState
	{
		bool bEnabled = false;
		EVariableShadingRate ShadingRate = EVariableShadingRate::e1x1;
		EShadingRateCombiner PipelinePrimitiveCombiner = EShadingRateCombiner::Passthrough;
		EShadingRateCombiner ImageCombiner = EShadingRateCombiner::Passthrough;

		bool operator ==(const FVariableRateShadingState& b) const
		{
			return bEnabled == b.bEnabled
				&& ShadingRate == b.ShadingRate
				&& PipelinePrimitiveCombiner == b.PipelinePrimitiveCombiner
				&& ImageCombiner == b.ImageCombiner;
		}

		bool operator !=(const FVariableRateShadingState& b) const { return !(*this == b); }

		constexpr FVariableRateShadingState& SetEnabled(bool value) { bEnabled = value; return *this; }
		constexpr FVariableRateShadingState& SetShadingRate(EVariableShadingRate value) { ShadingRate = value; return *this; }
		constexpr FVariableRateShadingState& SetPipelinePrimitiveCombiner(EShadingRateCombiner value) { PipelinePrimitiveCombiner = value; return *this; }
		constexpr FVariableRateShadingState& SetImageCombiner(EShadingRateCombiner value) { ImageCombiner = value; return *this; }
	};
	
    struct FGraphicsPipelineDesc
    {
        EPrimitiveType					PrimType = EPrimitiveType::TriangleList;
        uint32							PatchControlPoints = 0;
        FRHIInputLayoutRef				InputLayout;
        FRHIVertexShaderRef				VS;
        FRHIPixelShaderRef				PS;
        FRenderState					RenderState;
        FVariableRateShadingState		ShadingRateState;
        TVector<FRHIBindingLayoutRef>	BindingLayouts;
        
        FGraphicsPipelineDesc& SetPrimType(EPrimitiveType value) { PrimType = value; return *this; }
        FGraphicsPipelineDesc& SetPatchControlPoints(uint32 value) { PatchControlPoints = value; return *this; }
        FGraphicsPipelineDesc& SetInputLayout(IRHIInputLayout* value) { InputLayout = value; return *this; }
    	FGraphicsPipelineDesc& SetVertexShader(FRHIVertexShader* value) { VS = value; return *this; }
        FGraphicsPipelineDesc& SetPixelShader(FRHIPixelShader* value) { PS = value; return *this; }
        FGraphicsPipelineDesc& SetFragmentShader(FRHIPixelShader* value) { PS = value; return *this; }
        FGraphicsPipelineDesc& SetRenderState(const FRenderState& value) { RenderState = value; return *this; }
        FGraphicsPipelineDesc& SetVariableRateShadingState(const FVariableRateShadingState& value) { ShadingRateState = value; return *this; }
        FGraphicsPipelineDesc& AddBindingLayout(FRHIBindingLayout* layout) { BindingLayouts.push_back(layout); return *this; }
    };

	
	class LUMINA_API FRHIGraphicsPipeline : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_GraphicsPipeline)

		NODISCARD virtual const FGraphicsPipelineDesc& GetDesc() const = 0;
		
	};

	
	//-------------------------------------------------------------------------------------------------------------------


	struct LUMINA_API FComputePipelineDesc
	{
		FRHIComputeShaderRef			CS;
		TVector<FRHIBindingLayoutRef>	BindingLayouts;

		FComputePipelineDesc& SetComputeShader(FRHIComputeShader* value) { CS = value; return *this; }
		FComputePipelineDesc& AddBindingLayout(FRHIBindingLayout* layout) { BindingLayouts.push_back(layout); return *this; }
	};
	
	class LUMINA_API FRHIComputePipeline : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_ComputePipeline)

		NODISCARD virtual const FComputePipelineDesc& GetDesc() const = 0;
		
	};
	
}


namespace eastl
{
	using namespace Lumina;

	template<>
	struct hash<FVertexAttributeDesc>
	{
		size_t operator()(const FVertexAttributeDesc& Item) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Item.ArraySize);
			Hash::HashCombine(hash, Item.bInstanced);
			Hash::HashCombine(hash, Item.BufferIndex);
			Hash::HashCombine(hash, Item.ElementStride);
			Hash::HashCombine(hash, (uint32)Item.Format);
			Hash::HashCombine(hash, Item.Offset);

			return hash;
		}
	};
	
	template<>
	struct hash<FBindingSetItem>
	{
		size_t operator()(const FBindingSetItem& Item) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Item.ResourceHandle);
			Hash::HashCombine(hash, Item.bUnused);
			Hash::HashCombine(hash, Item.Slot);
			Hash::HashCombine(hash, Item.Type);

			return hash;
		}
	};

	template<> 
	struct hash<FSamplerDesc> 
	{ 
		size_t operator()(const FSamplerDesc& Item) const 
		{ 
			size_t hash = 0; 
			//Hash::HashCombine(hash, Item.BorderColor);
			Hash::HashCombine(hash, Item.MaxAnisotropy);
			Hash::HashCombine(hash, Item.MipBias);
			Hash::HashCombine(hash, Item.MinFilter);
			Hash::HashCombine(hash, Item.MagFilter);
			Hash::HashCombine(hash, Item.MipFilter);
			Hash::HashCombine(hash, static_cast<int>(Item.AddressU));
			Hash::HashCombine(hash, static_cast<int>(Item.AddressV));
			Hash::HashCombine(hash, static_cast<int>(Item.AddressW));
			Hash::HashCombine(hash, static_cast<int>(Item.ReductionType));
			return hash; 
		} 
	};

	template<>
	struct hash<FBindingLayoutItem>
	{
		size_t operator()(const FBindingLayoutItem& Item) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Item.Size);
			Hash::HashCombine(hash, Item.Slot);
			Hash::HashCombine(hash, Item.Type);

			return hash;
		}
	};

	template<>
	struct hash<FBindingLayoutDesc>
	{
		size_t operator()(const FBindingLayoutDesc& Item) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Item.Index);
			Hash::HashCombine(hash, Item.StageFlags.Get());
			for (const FBindingLayoutItem& LayoutItem : Item.Bindings)
			{
				Hash::HashCombine(hash, LayoutItem);
			}

			return hash;
		}
	};

	template<>
	struct hash<FBindingSetDesc>
	{
		size_t operator()(const FBindingSetDesc& Item) const
		{
			size_t hash = 0;
			for (const FBindingSetItem& LayoutItem : Item.Bindings)
			{
				Hash::HashCombine(hash, LayoutItem);
			}

			return hash;
		}
	};

	template<>
	struct hash<FBlendState>
	{
		size_t operator()(const FBlendState& State) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, State.AlphaToCoverageEnable);

			for (uint32_t i = 0; i < MaxRenderTargets; ++i)
			{
				Hash::HashCombine(hash, State.Targets[i].bBlendEnable);
				Hash::HashCombine(hash, (State.Targets[i].SrcBlend));
				Hash::HashCombine(hash, (State.Targets[i].DestBlend));
				Hash::HashCombine(hash, (State.Targets[i].BlendOp));
				Hash::HashCombine(hash, (State.Targets[i].SrcBlendAlpha));
				Hash::HashCombine(hash, (State.Targets[i].DestBlendAlpha));
				Hash::HashCombine(hash, (State.Targets[i].BlendOpAlpha));
				Hash::HashCombine(hash, (State.Targets[i].ColorWriteMask));
			}

			return hash;
		}
	};


	template<>
	struct hash<FSinglePassStereoState>
	{
		size_t operator()(const FSinglePassStereoState& State) const
		{
			size_t hash = 0;
            
			Hash::HashCombine(hash, State.bEnabled);
			Hash::HashCombine(hash, State.bIndependentViewportMask);
			Hash::HashCombine(hash, State.RenderTargetIndexOffset);

			return hash;
		}
	};

	
	template<>
	struct hash<FRasterState>
	{
		size_t operator()(const FRasterState& State) const
		{
			size_t hash = 0;
            
			Hash::HashCombine(hash, static_cast<uint32>(State.FillMode));
			Hash::HashCombine(hash, static_cast<uint32>(State.CullMode));
			Hash::HashCombine(hash, State.FrontCounterClockwise);
			Hash::HashCombine(hash, State.DepthClipEnable);
			Hash::HashCombine(hash, State.ScissorEnable);
			Hash::HashCombine(hash, State.MultisampleEnable);
			Hash::HashCombine(hash, State.AntialiasedLineEnable);
			Hash::HashCombine(hash, State.DepthBias);
			Hash::HashCombine(hash, State.DepthBiasClamp);
			Hash::HashCombine(hash, State.SlopeScaledDepthBias);
			Hash::HashCombine(hash, State.ForcedSampleCount);
			Hash::HashCombine(hash, State.ProgrammableSamplePositionsEnable);
			Hash::HashCombine(hash, State.ConservativeRasterEnable);
			Hash::HashCombine(hash, State.QuadFillEnable);

			for (size_t i = 0; i < 16; ++i)
			{
				Hash::HashCombine(hash, State.SamplePositionsX[i]);
				Hash::HashCombine(hash, State.SamplePositionsY[i]);
			}

			return hash;
		}
	};
	
	template<>
	struct hash<FRenderState>
	{
		size_t operator()(const FRenderState& State) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, State.BlendState);
			Hash::HashCombine(hash, State.DepthStencilState);
			Hash::HashCombine(hash, State.RasterState);
			Hash::HashCombine(hash, State.SinglePassStereo);
			return hash;
		}
	};
	
	
	template<>
	struct hash<FVariableRateShadingState>
	{
		size_t operator()(const FVariableRateShadingState& State) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, State.bEnabled);
			Hash::HashCombine(hash, static_cast<uint32>(State.ShadingRate));
			Hash::HashCombine(hash, static_cast<uint32>(State.PipelinePrimitiveCombiner));
			Hash::HashCombine(hash, static_cast<uint32>(State.ImageCombiner));
			return hash;
		}
	};

	template<>
	struct hash<FDepthStencilState>
	{
		size_t operator()(const FDepthStencilState& State) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, (uint32)State.DepthFunc);
			Hash::HashCombine(hash, State.StencilEnable);
			Hash::HashCombine(hash, State.BackFaceStencil.GetHash());
			Hash::HashCombine(hash, State.DepthTestEnable);
			Hash::HashCombine(hash, State.DepthWriteEnable);
			Hash::HashCombine(hash, State.StencilReadMask);
			Hash::HashCombine(hash, State.StencilRefValue);
			Hash::HashCombine(hash, State.StencilWriteMask);
			return hash;
		}
	};

	template<>
	struct hash<FGraphicsPipelineDesc>
	{
		size_t operator()(const FGraphicsPipelineDesc& Desc) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Desc.VS ? Desc.VS->GetKey() : 0);
			Hash::HashCombine(hash, Desc.PS ? Desc.PS->GetKey() : 0);
			Hash::HashCombine(hash, Desc.RenderState);
			return hash;
		}
	};

	template<>
	struct hash<FComputePipelineDesc>
	{
		size_t operator()(const FComputePipelineDesc& Desc) const
		{
			size_t hash = 0;
			Hash::HashCombine(hash, Desc.CS->GetKey());
			return hash;
		}
	};
}