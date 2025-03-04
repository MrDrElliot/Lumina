#pragma once

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
	static constexpr uint32 MaxBindingLayouts = 5;
	static constexpr uint32 MaxBindingsPerLayout = 128;

/** An enumeration of the different RHI reference types. */
enum ERHIResourceType : uint8
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexShader,
	RRT_PixelShader,
	RRT_ComputeShader,
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
    class IRHIResource
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
    		FMemory::Delete(MutableThis);
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


	class FRHIViewport : public IRHIResource
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
		FORCEINLINE virtual FRHIImageRef GetRenderTarget() const { return RenderTarget; };

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
    
	enum class EBlendOp : uint8
	{
		Add = 1,
		Subrtact = 2,
		ReverseSubtract = 3,
		Min = 4,
		Max = 5
	};

	enum class EColorMask : uint8
	{
		// These values are equal to their counterparts in DX11, DX12, and Vulkan.
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 0xF
	};

	ENUM_BITSET(EColorMask)

	struct FVertexAttributeDesc
	{
		FString Name;
		EImageFormat Format = EImageFormat::None;
		uint32 ArraySize = 1;
		uint32 BufferIndex = 0;
		uint32 Offset = 0;
		uint32 ElementStride = 0;
		bool bInstanced = false;

		FVertexAttributeDesc& SetName(const FString& value) { Name = value; return *this; }
		constexpr FVertexAttributeDesc& setFormat(EImageFormat value) { Format = value; return *this; }
		constexpr FVertexAttributeDesc& setArraySize(uint32 value) { ArraySize = value; return *this; }
		constexpr FVertexAttributeDesc& setBufferIndex(uint32 value) { BufferIndex = value; return *this; }
		constexpr FVertexAttributeDesc& setOffset(uint32 value) { Offset = value; return *this; }
		constexpr FVertexAttributeDesc& setElementStride(uint32 value) { ElementStride = value; return *this; }
		constexpr FVertexAttributeDesc& setIsInstanced(bool value) { bInstanced = value; return *this; }
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
            bool			BlendEnable = false;
            EBlendFactor 	SrcBlend = EBlendFactor::One;
            EBlendFactor 	DestBlend = EBlendFactor::Zero;
            EBlendOp     	BlendOp = EBlendOp::Add;
            EBlendFactor 	SrcBlendAlpha = EBlendFactor::One;
            EBlendFactor 	DestBlendAlpha = EBlendFactor::Zero;
            EBlendOp     	BlendOpAlpha = EBlendOp::Add;
            EColorMask   	ColorWriteMask = EColorMask::All;

            constexpr RenderTarget& SetBlendEnable(bool enable) { BlendEnable = enable; return *this; }
            constexpr RenderTarget& EnableBlend() { BlendEnable = true; return *this; }
            constexpr RenderTarget& DisableBlend() { BlendEnable = false; return *this; }
            constexpr RenderTarget& SetSrcBlend(EBlendFactor value) { SrcBlend = value; return *this; }
            constexpr RenderTarget& SetDestBlend(EBlendFactor value) { DestBlend = value; return *this; }
            constexpr RenderTarget& SetBlendOp(EBlendOp value) { BlendOp = value; return *this; }
            constexpr RenderTarget& SetSrcBlendAlpha(EBlendFactor value) { SrcBlendAlpha = value; return *this; }
            constexpr RenderTarget& SetDestBlendAlpha(EBlendFactor value) { DestBlendAlpha = value; return *this; }
            constexpr RenderTarget& SetBlendOpAlpha(EBlendOp value) { BlendOpAlpha = value; return *this; }
            constexpr RenderTarget& SetColorWriteMask(EColorMask value) { ColorWriteMask = value; return *this; }
        	
            constexpr bool operator ==(const RenderTarget& other) const
            {
                return BlendEnable == other.BlendEnable
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

        constexpr FDepthStencilState& setDepthTestEnable(bool value) { DepthTestEnable = value; return *this; }
        constexpr FDepthStencilState& enableDepthTest() { DepthTestEnable = true; return *this; }
        constexpr FDepthStencilState& disableDepthTest() { DepthTestEnable = false; return *this; }
        constexpr FDepthStencilState& setDepthWriteEnable(bool value) { DepthWriteEnable = value; return *this; }
        constexpr FDepthStencilState& enableDepthWrite() { DepthWriteEnable = true; return *this; }
        constexpr FDepthStencilState& disableDepthWrite() { DepthWriteEnable = false; return *this; }
        constexpr FDepthStencilState& setDepthFunc(EComparisonFunc value) { DepthFunc = value; return *this; }
        constexpr FDepthStencilState& setStencilEnable(bool value) { StencilEnable = value; return *this; }
        constexpr FDepthStencilState& enableStencil() { StencilEnable = true; return *this; }
        constexpr FDepthStencilState& disableStencil() { StencilEnable = false; return *this; }
        constexpr FDepthStencilState& setStencilReadMask(uint8 value) { StencilReadMask = value; return *this; }
        constexpr FDepthStencilState& setStencilWriteMask(uint8 value) { StencilWriteMask = value; return *this; }
        constexpr FDepthStencilState& setStencilRefValue(uint8 value) { StencilRefValue = value; return *this; }
        constexpr FDepthStencilState& setFrontFaceStencil(const StencilOpDesc& value) { FrontFaceStencil = value; return *this; }
        constexpr FDepthStencilState& setBackFaceStencil(const StencilOpDesc& value) { BackFaceStencil = value; return *this; }
        constexpr FDepthStencilState& setDynamicStencilRef(bool value) { DynamicStencilRef = value; return *this; }
        
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

	struct FBindingSetItem
	{
		IRHIResource* ResourceHandle;

		uint32 Slot;

		ERHIResourceType Type		: 8;
		EImageDimension Dimension	: 8; // valid for Texture_SRV, Texture_UAV
		EImageFormat Format			: 8; // valid for Texture_SRV, Texture_UAV, Buffer_SRV, Buffer_UAV
		uint8 bUnused				: 8;

		union 
		{
			uint64		RawData[2];
		};

		bool operator ==(const FBindingSetItem& b) const
		{
			return ResourceHandle == b.ResourceHandle
				&& Slot == b.Slot
				&& Type == b.Type
				&& Dimension == b.Dimension
				&& Format == b.Format
				&& RawData[0] == b.RawData[0]
				&& RawData[1] == b.RawData[1];
		}

	};
	
	struct FBindingSetDesc
	{
		TInlineVector<FBindingSetItem, MaxBindingsPerLayout> Bindings;
		
		bool bTrackLiveness = true;

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
		FBindingSetDesc& SetTrackLiveness(bool value) { bTrackLiveness = value; return *this; }
	};
	
	class FRHIBindingLayout : public IRHIResource
	{
	public:
		
		NODISCARD virtual const FBindingSetDesc* GetDesc() const = 0;
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
        FVariableRateShadingState		shadingRateState;
        TVector<FRHIBindingLayoutRef>	BindingLayouts;
        
        FGraphicsPipelineDesc& SetPrimType(EPrimitiveType value) { PrimType = value; return *this; }
        FGraphicsPipelineDesc& SetPatchControlPoints(uint32 value) { PatchControlPoints = value; return *this; }
        FGraphicsPipelineDesc& SetInputLayout(IRHIInputLayout* value) { InputLayout = value; return *this; }
        FGraphicsPipelineDesc& SetPixelShader(FRHIPixelShader* value) { PS = value; return *this; }
        FGraphicsPipelineDesc& SetFragmentShader(FRHIPixelShader* value) { PS = value; return *this; }
        FGraphicsPipelineDesc& SetRenderState(const FRenderState& value) { RenderState = value; return *this; }
        FGraphicsPipelineDesc& SetVariableRateShadingState(const FVariableRateShadingState& value) { shadingRateState = value; return *this; }
        FGraphicsPipelineDesc& AddBindingLayout(FRHIBindingLayout* layout) { BindingLayouts.push_back(layout); return *this; }
    };

	
	class FRHIGraphicsPipeline : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_GraphicsPipeline)
		
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

	
}


