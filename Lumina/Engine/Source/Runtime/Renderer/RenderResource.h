#pragma once

#include "RenderTypes.h"
#include "Containers/String.h"
#include "Memory/RefCounted.h"
#include "Types/BitFlags.h"


namespace Lumina
{
	enum class EBufferUsageFlags : uint32;
    class IRenderContext;
}


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
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
	RRT_Image,
	RRT_GPUFence,
	RRT_Viewport,
	RRT_StagingBuffer,


	RRT_Num
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
    	
    };


	//-------------------------------------------------------------------------------------------------------------------

	struct FRHIBufferDesc
	{
		uint32 Size = 0;
		uint32 Stride = 0;
		TBitFlags<EBufferUsageFlags> Usage = 0;

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

	
	class FRHIBuffer : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Buffer);

		FRHIBuffer(const FRHIBufferDesc& InDesc)
			:Description(InDesc)
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

	
	class FRHIImage : public IRHIResource
	{
	public:

		RENDER_RESOURCE(RRT_Image)

		FRHIImage(const FRHIImageDesc& InDesc);


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

		FORCEINLINE uint32 GetHash() const { return Hash; }
		void SetHash(uint32 InHash) { Hash = InHash; }
	
	private:

		uint32 Hash = 0;
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
	
}

