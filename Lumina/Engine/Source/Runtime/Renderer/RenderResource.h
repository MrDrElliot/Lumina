#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderTypes.h"
#include "StateTracking.h"
#include "ViewVolume.h"
#include "Containers/String.h"
#include "Core/LuminaMacros.h"
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


enum class EFormat : uint8
{
	UNKNOWN,

	R8_UINT,
	R8_SINT,
	R8_UNORM,
	R8_SNORM,
	RG8_UINT,
	RG8_SINT,
	RG8_UNORM,
	RG8_SNORM,
	R16_UINT,
	R16_SINT,
	R16_UNORM,
	R16_SNORM,
	R16_FLOAT,
	BGRA4_UNORM,
	B5G6R5_UNORM,
	B5G5R5A1_UNORM,
	RGBA8_UINT,
	RGBA8_SINT,
	RGBA8_UNORM,
	RGBA8_SNORM,
	BGRA8_UNORM,
	SRGBA8_UNORM,
	SBGRA8_UNORM,
	R10G10B10A2_UNORM,
	R11G11B10_FLOAT,
	RG16_UINT,
	RG16_SINT,
	RG16_UNORM,
	RG16_SNORM,
	RG16_FLOAT,
	R32_UINT,
	R32_SINT,
	R32_FLOAT,
	RGBA16_UINT,
	RGBA16_SINT,
	RGBA16_FLOAT,
	RGBA16_UNORM,
	RGBA16_SNORM,
	RG32_UINT,
	RG32_SINT,
	RG32_FLOAT,
	RGB32_UINT,
	RGB32_SINT,
	RGB32_FLOAT,
	RGBA32_UINT,
	RGBA32_SINT,
	RGBA32_FLOAT,
        
	D16,
	D24S8,
	X24G8_UINT,
	D32,
	D32S8,
	X32G8_UINT,

	BC1_UNORM,
	BC1_UNORM_SRGB,
	BC2_UNORM,
	BC2_UNORM_SRGB,
	BC3_UNORM,
	BC3_UNORM_SRGB,
	BC4_UNORM,
	BC4_SNORM,
	BC5_UNORM,
	BC5_SNORM,
	BC6H_UFLOAT,
	BC6H_SFLOAT,
	BC7_UNORM,
	BC7_UNORM_SRGB,

	COUNT,
};

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
	uint8_t BytesPerBlock;
	uint8_t BlockSize;
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


	RRT_Num
};

enum class ERHIBindingResourceType : uint8
{
	// SRV (Shader Resource View) Types.
	Texture_SRV,
	Buffer_SRV,


	// UAV (Unordered Access View) Types.
	Texture_UAV,
	Buffer_UAV,

	// Constant Buffer (could be a uniform buffer)
	Buffer_CBV,
	Buffer_Uniform = Buffer_CBV,
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
	Default,
	Image,
	ImageView,

	CommandBuffer,
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
    class LUMINA_API IRHIResource
    {
    public:

    	virtual ERHIResourceType GetResourceType() const = 0;

        IRHIResource() = default;
        virtual ~IRHIResource() = default;

    	template<typename T, EAPIResourceType Type = EAPIResourceType::Default>
		T GetAPIResource()
    	{
    		void* Resource = GetAPIResourceImpl(Type);
    		return static_cast<T>(Resource);
    	}

		#if LE_DEBUG
		void SetDebugName(const FString& InName) { DebugName = InName; }
		#endif

    	
    protected:

    	virtual void* GetAPIResourceImpl(EAPIResourceType Type) { return nullptr; }

    private:

    	void Destroy() const;
    	
    public:

    	        
    	FORCEINLINE uint32 AddRef() const
    	{
    		int32 NewValue = AtomicFlags.AddRef(std::memory_order_acquire);
    		Assert(NewValue > 0); 
    		return uint32(NewValue);
    	}
        
    	FORCEINLINE uint32 Release() const
    	{
    		int32 NewValue = AtomicFlags.Release(std::memory_order_release);
    		Assert(NewValue >= 0);

    		if (NewValue == 0)
    		{
    			Destroy();
    		}
    		
    		Assert(NewValue >= 0);
    		return uint32(NewValue);
    	}
        
    	FORCEINLINE uint32 GetRefCount() const
    	{
    		int32 CurrentValue = AtomicFlags.GetNumRefs(std::memory_order_relaxed);
    		Assert(CurrentValue >= 0); 
    		return uint32(CurrentValue);
    	}

    	bool IsValid() const
    	{
    		return AtomicFlags.IsValid(std::memory_order_relaxed);
    	}

    	void Delete()
    	{
    		Assert(!AtomicFlags.MarkForDelete(std::memory_order_acquire));
    		IRHIResource* MutableThis = this;
    		Memory::Delete(MutableThis);
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
				Assert((LocalPacked & MarkedForDeleteBit) != 0);
				Assert((LocalPacked & DeletingBit) == 0);
				uint32 NumRefs = LocalPacked & NumRefsMask;
		
				if (NumRefs == 0) // caches can bring dead objects back to life
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

		#if LE_DEBUG
		FString DebugName;
		#endif
    	
    };

	

	//-------------------------------------------------------------------------------------------------------------------


	class LUMINA_API FRHIViewport : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Viewport)
		
		FRHIViewport(const FIntVector2D& InSize, IRenderContext* InContext)
			: RenderContext(InContext)
			, ViewVolume()
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
		

		FORCEINLINE const FViewVolume& GetViewVolume() const { return ViewVolume; }
		FORCEINLINE const FIntVector2D& GetSize() const { return Size; }

		FORCEINLINE void SetViewVolume(const FViewVolume& InVolume) { ViewVolume = InVolume;}
		FORCEINLINE virtual FRHIImageRef GetRenderTarget() const { return RenderTarget; }

	private:

		void CreateRenderTarget(const FIntVector2D& InSize);

	protected:

		IRenderContext*		RenderContext;
		FRHIImageRef		RenderTarget;
		FViewVolume        	ViewVolume;
		FIntVector2D       	Size; 
    
	};


	//-------------------------------------------------------------------------------------------------------------------

	

	struct FRHIBufferDesc
	{
		uint32 Size = 0;
		uint32 Stride = 0;
		TBitFlags<EBufferUsageFlags> Usage;

		FRHIBufferDesc() = default;
		FRHIBufferDesc(uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
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
				Assert(Size == 0 && Stride == 0 && Usage.IsFlagSet(BUF_NullResource));
				return true;
			}

			return false;
		}
	};

	
	class FRHIBuffer : public IRHIResource, public IAccessableRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Buffer)

		FRHIBuffer(const FRHIBufferDesc& InDesc)
			: Description(InDesc)
		{}
		
		FORCEINLINE const FRHIBufferDesc& GetDescription() const { return Description; }

		FORCEINLINE bool IsStorageBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::StorageBuffer); }
		FORCEINLINE bool IsUniformBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::UniformBuffer); }
		FORCEINLINE bool IsVertexBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::VertexBuffer); }
		FORCEINLINE bool IsIndexBuffer() const { return Description.Usage.IsFlagSet(EBufferUsageFlags::IndexBuffer); }

		
		FORCEINLINE uint32 GetSize() const { return Description.Size; }
		FORCEINLINE uint32 GetStride() const { return Description.Stride; }
		FORCEINLINE TBitFlags<EBufferUsageFlags> GetUsage() const { return Description.Usage; }

	private:

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
		EImageFormat Format = EImageFormat::None;
		
	};

	enum class ESamplerAddressMode : uint8
	{
		// D3D names
		Clamp,
		Wrap,
		Border,
		Mirror,
		MirrorOnce,

		// Vulkan names
		ClampToEdge = Clamp,
		Repeat = Wrap,
		ClampToBorder = Border,
		MirroredRepeat = Mirror,
		MirrorClampToEdge = MirrorOnce
	};

	enum class ESamplerReductionType : uint8_t
	{
		Standard,
		Comparison,
		Minimum,
		Max
	};

	struct FSamplerDesc
	{
		FColor BorderColor = 1.f;
		float MaxAnisotropy = 1.f;
		float MipBias = 0.f;

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

	class FRHISampler : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_SamplerState)

		NODISCARD virtual const FSamplerDesc& GetDesc() const = 0;

		
	};
	
	class FRHIImage : public IRHIResource, public IAccessableRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Image)

		FRHIImage(const FRHIImageDesc& InDesc)
			: Description(InDesc)
		{}
		
		FORCEINLINE const FRHIImageDesc& GetDescription() const { return Description; }
		
		FORCEINLINE const FIntVector2D& GetExtent() const { return Description.Extent; }
		FORCEINLINE uint32 GetSizeX() const { return Description.Extent.X; }
		FORCEINLINE uint32 GetSizeY() const { return Description.Extent.Y; }
		FORCEINLINE EImageFormat GetFormat() const { return Description.Format; }
		FORCEINLINE TBitFlags<EImageCreateFlags> GetFlags() const { return Description.Flags; }
		
	
	private:

		FRHIImageDesc Description;
	};

	//-------------------------------------------------------------------------------------------------------------------

	class FRHIShader : public IRHIResource
	{
	public:

		FORCEINLINE FName GetKey() const { return Key; }
		void SetKey(FName InKey) { Key = InKey; }
		
		/** Get the shader's native representation of it's bytecode */
		virtual void GetByteCode(const void** ByteCode, uint64* Size) = 0;
	
	private:
		
		FName Key;
	};

	class FRHIVertexShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_VertexShader)
		
	};

	class FRHIPixelShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_PixelShader)
		
	};
	
	class FRHIComputeShader : public FRHIShader
	{
	public:

		RENDER_RESOURCE(RRT_ComputeShader)
		
	};

	class FShaderLibrary : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_ShaderLibrary)

		void AddShader(FRHIShader* Shader);
		void RemoveShader(FName Key);

		template<typename T>
		T GetShader(FName Key)
		{
			return GetShader(Key).As<T>();
		}

		FRHIShaderRef GetShader(FName Key);

	private:

		THashMap<FName, FRHIShaderRef> Shaders;
    
	};
	
	//-------------------------------------------------------------------------------------------------------------------

	enum class ERasterFillMode : uint8_t
	{
		Solid,
		Wireframe,

		// Vulkan names
		Fill = Solid,
		Line = Wireframe
	};

	enum class ERasterCullMode : uint8
	{
		Back,
		Front,
		None
	};

	enum class EBlendFactor : uint8
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

	struct FVertexAttributeDesc
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

	class IRHIInputLayout : public IRHIResource
	{
	public:
		
		NODISCARD virtual uint32 GetNumAttributes() const = 0;
		NODISCARD virtual const FVertexAttributeDesc* GetAttributeDesc(uint32 index) const = 0;
	};
	
	struct FBlendState
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
        	
            constexpr bool operator ==(const RenderTarget& other) const
            {
                return bBlendEnable == other.bBlendEnable
                    && SrcBlend == other.SrcBlend
                    && DestBlend == other.DestBlend
                    && BlendOp == other.BlendOp
                    && SrcBlendAlpha == other.SrcBlendAlpha
                    && DestBlendAlpha == other.DestBlendAlpha
                    && BlendOpAlpha == other.BlendOpAlpha
                    && ColorWriteMask == other.ColorWriteMask;
            }

            constexpr bool operator !=(const RenderTarget& other) const
            {
                return !(*this == other);
            }
        };

        RenderTarget Targets[MaxRenderTargets];
        bool AlphaToCoverageEnable = false;

        constexpr FBlendState& SetRenderTarget(uint32 index, const RenderTarget& target) { Targets[index] = target; return *this; }
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

	
	struct FRasterState
    {
        ERasterFillMode FillMode = ERasterFillMode::Solid;
        ERasterCullMode CullMode = ERasterCullMode::Back;
        bool FrontCounterClockwise = false;
        bool DepthClipEnable = false;
        bool ScissorEnable = false;
        bool MultisampleEnable = false;
        bool AntialiasedLineEnable = false;
        int DepthBias = 0;
        float DepthBiasClamp = 0.f;
        float SlopeScaledDepthBias = 0.f;


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

	enum class EStencilOp : uint8
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

	enum class EComparisonFunc : uint8
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
	
	struct FDepthStencilState
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

	struct FSinglePassStereoState
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
	
	struct FRenderState
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

	struct FBindingLayoutItem
	{
		uint32 Slot;
		
		ERHIBindingResourceType Type	: 8;
		uint16 Size						: 16;
	};

	struct FBindingLayoutDesc
	{
		TBitFlags<ERHIShaderType> StageFlags;
		uint32 Index = 0;

		TFixedVector<FBindingLayoutItem, MaxBindingsPerLayout> Bindings;

		
		FBindingLayoutDesc& SetVisibility(ERHIShaderType InType) { StageFlags.SetFlag(InType); return *this; }
		FBindingLayoutDesc& SetBindingIndex(uint32 Value) { Index = Value; return *this; }
		FBindingLayoutDesc& AddItem(const FBindingLayoutItem& Item) { Bindings.push_back(Item); return *this; }
	};

	struct FBindingSetItem
	{
		IRHIResource* ResourceHandle;
		uint32 Slot;
		
		ERHIBindingResourceType Type	: 8;
		uint8 bUnused					: 8;

		union 
		{
			uint64		RawData[2];
		};

		static FBindingSetItem BufferSRV(uint32 Slot, FRHIBuffer* Buffer)
		{
			FBindingSetItem Result;
			Result.bUnused = false;
			Result.Type = ERHIBindingResourceType::Buffer_SRV;
			Result.Slot = Slot;
			Result.ResourceHandle = Buffer;
			memset(Result.RawData, 0, std::size(Result.RawData));
			
			return Result;
		}

		static FBindingSetItem BufferUAV(uint32 Slot, FRHIBuffer* Buffer)
		{
			FBindingSetItem Result;
			Result.bUnused = false;
			Result.Type = ERHIBindingResourceType::Buffer_UAV;
			Result.Slot = Slot;
			Result.ResourceHandle = Buffer;
			memset(Result.RawData, 0, std::size(Result.RawData));
			
			return Result;
		}
		
		static FBindingSetItem BufferCBV(uint32 Slot, FRHIBuffer* Buffer)
		{
			FBindingSetItem Result;
			Result.bUnused = false;
			Result.Type = ERHIBindingResourceType::Buffer_Uniform;
			Result.Slot = Slot;
			Result.ResourceHandle = Buffer;
			memset(Result.RawData, 0, std::size(Result.RawData));
			
			return Result;
		}

		
		static FBindingSetItem TextureSRV(uint32 Slot, FRHIImage* Image)
		{
			FBindingSetItem Result;
			Result.bUnused = false;
			Result.Type = ERHIBindingResourceType::Texture_SRV;
			Result.Slot = Slot;
			Result.ResourceHandle = Image;
			memset(Result.RawData, 0, std::size(Result.RawData));
			
			return Result;
		}

		static FBindingSetItem TextureUAV(uint32 Slot, FRHIImage* Image)
		{
			FBindingSetItem Result;
			Result.bUnused = false;
			Result.Type = ERHIBindingResourceType::Texture_UAV;
			Result.Slot = Slot;
			Result.ResourceHandle = Image;
			memset(Result.RawData, 0, std::size(Result.RawData));
			
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
	
	struct FBindingSetDesc
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

	
	class FRHIBindingLayout : public IRHIResource
	{
	public:
		
		NODISCARD virtual const FBindingLayoutDesc* GetDesc() const = 0;
	};

	class FRHIBindingSet : public IRHIResource
	{
	public:
		
		NODISCARD virtual const FBindingSetDesc* GetDesc() const = 0;
		NODISCARD virtual FRHIBindingLayout* GetLayout() const = 0;
		
	};

	enum class EVariableShadingRate : uint8
	{
		e1x1,
		e1x2,
		e2x1,
		e2x2,
		e2x4,
		e4x2,
		e4x4
	};

	enum class EShadingRateCombiner : uint8
	{
		Passthrough,
		Override,
		Min,
		Max,
		ApplyRelative
	};

	struct FVariableRateShadingState
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

	
	class FRHIGraphicsPipeline : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_GraphicsPipeline)

		NODISCARD virtual const FGraphicsPipelineDesc& GetDesc() const = 0;
		
	};

	
	//-------------------------------------------------------------------------------------------------------------------


	struct FComputePipelineDesc
	{
		FRHIComputeShaderRef			CS;
		TVector<FRHIBindingLayoutRef>	BindingLayouts;

		FComputePipelineDesc& SetComputeShader(FRHIComputeShader* value) { CS = value; return *this; }
		FComputePipelineDesc& AddBindingLayout(FRHIBindingLayout* layout) { BindingLayouts.push_back(layout); return *this; }
	};
	
	class FRHIComputePipeline : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_ComputePipeline)

		NODISCARD virtual const FComputePipelineDesc& GetDesc() const = 0;
		
	};


	struct FGraphicsState
	{

		FRHIGraphicsPipeline*	Pipeline;
		FRHIViewport*			Viewport;

		FGraphicsState& SetPipeline(FRHIGraphicsPipeline* InPipeline) { Pipeline = InPipeline; return *this; }
		FGraphicsState& SetViewport(FRHIViewport* InViewport) { Viewport = InViewport; return *this; }
		
	};

	struct FComputeState
	{
		FRHIComputePipeline*	Pipeline;

		FComputeState& SetPipeline(FRHIComputePipeline* InPipeline) { Pipeline = InPipeline; return *this; }
	};
	
	
}


namespace eastl
{
	using namespace Lumina;

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
			Hash::HashCombine(hash, Desc.VS->GetKey());
			Hash::HashCombine(hash, Desc.PS->GetKey());
			
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