#include "VulkanBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Swapchain.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"
#include "glm/glm.hpp"
#include "Math/Alignment.h"

namespace Lumina
{
	VkBufferUsageFlags convert(EDeviceBufferUsage Usage) 
	{
		switch (Usage)
		{
			case EDeviceBufferUsage::VERTEX_BUFFER:				    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case EDeviceBufferUsage::INDEX_BUFFER:					return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case EDeviceBufferUsage::UNIFORM_BUFFER:				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			case EDeviceBufferUsage::UNIFORM_BUFFER_DYNAMIC:		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			case EDeviceBufferUsage::STORAGE_BUFFER:				return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			case EDeviceBufferUsage::STORAGE_BUFFER_DYNAMIC:		return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			case EDeviceBufferUsage::STAGING_BUFFER:				return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			case EDeviceBufferUsage::SHADER_DEVICE_ADDRESS:			return VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			default:												std::unreachable();
		}
	}

	VmaAllocationCreateFlags convert(EDeviceBufferMemoryUsage Usage)
	{
		switch (Usage)
		{
			case EDeviceBufferMemoryUsage::READ_BACK:				return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;			
			case EDeviceBufferMemoryUsage::COHERENT_WRITE:			return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;  
			case EDeviceBufferMemoryUsage::NO_HOST_ACCESS:			return 0;														
			default:												std::unreachable();												
		}
	}
    
	FVulkanBuffer::FVulkanBuffer(const FDeviceBufferSpecification& Spec)
	: Buffer(VK_NULL_HANDLE)
	{
		Specification = Spec;
		
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		VmaAllocationCreateFlags VmaFlags = convert(Spec.MemoryUsage);
		
		
		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.size = GetAlignedSizeForBuffer(Spec.Size, Spec.BufferUsage);
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferCreateInfo.usage = convert(Spec.BufferUsage);
		BufferCreateInfo.pNext = nullptr;
		BufferCreateInfo.flags = 0;

		Allocation = Alloc->AllocateBuffer(&BufferCreateInfo, VmaFlags, &Buffer, Spec.DebugName);
		
	}

	FVulkanBuffer::FVulkanBuffer(const FDeviceBufferSpecification& Spec, void* InData, uint64 DataSize)
	: Buffer(VK_NULL_HANDLE)
	{
		Specification = Spec;
		
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		VmaAllocationCreateFlags AllocationCreateFlags = convert(Spec.MemoryUsage);

		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.size = GetAlignedSizeForBuffer(Spec.Size, Spec.BufferUsage);
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferCreateInfo.usage = convert(Spec.BufferUsage);
		BufferCreateInfo.pNext = nullptr;
		BufferCreateInfo.flags = 0;

		if (Specification.MemoryUsage == EDeviceBufferMemoryUsage::NO_HOST_ACCESS)
		{
			BufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		Allocation = Alloc->AllocateBuffer(&BufferCreateInfo, AllocationCreateFlags, &Buffer, Spec.DebugName);

		FVulkanBuffer::UploadData(0, InData, DataSize);
	}

	FVulkanBuffer::~FVulkanBuffer()
	{
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		Alloc->DestroyBuffer(Buffer, Allocation);
	}

	void FVulkanBuffer::SetFriendlyName(const FString& InName)
	{
		FBuffer::SetFriendlyName(InName);

		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
		VkDevice Device = RenderContext->GetDevice();
        
		VkDebugUtilsObjectNameInfoEXT NameInfo = {};
		NameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		NameInfo.pObjectName = GetFriendlyName().c_str();
		NameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		NameInfo.objectHandle = reinterpret_cast<uint64_t>(Buffer);


		RenderContext->GetRenderContextFunctions().DebugUtilsObjectNameEXT(Device, &NameInfo);
	}

	FDeviceBufferSpecification FVulkanBuffer::GetSpecification() const
	{
		return Specification;
	}

	void FVulkanBuffer::UploadData(uint64 Offset, void* InData, uint64 DataSize)
	{
		AssertMsg(InData, "Cannot upload empty data to a Vulkan Buffer!");

		if (Specification.MemoryUsage == EDeviceBufferMemoryUsage::NO_HOST_ACCESS) 
		{
			FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		

		
			FDeviceBufferSpecification StagingBufferSpec = {};
			StagingBufferSpec.Size = DataSize;
			StagingBufferSpec.BufferUsage = EDeviceBufferUsage::STAGING_BUFFER;
			StagingBufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
			StagingBufferSpec.DebugName = "Staging Buffer";
		
			TRefPtr<FVulkanBuffer> StagingBuffer = MakeRefPtr<FVulkanBuffer>(StagingBufferSpec, InData, DataSize);
		
			VkBufferCopy BufferCopy = {};
			BufferCopy.size = DataSize;
			BufferCopy.srcOffset = 0;
			BufferCopy.dstOffset = 0;
		
			VkCommandBuffer CmdBuffer = RenderContext->AllocateTransientCommandBuffer();
		
			vkCmdCopyBuffer(CmdBuffer, StagingBuffer->GetBuffer(), Buffer, 1, &BufferCopy);
		
			VkBufferMemoryBarrier BufferMemoryBarrier = {};
			BufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			BufferMemoryBarrier.buffer = Buffer;
			BufferMemoryBarrier.offset = 0;
			BufferMemoryBarrier.size = DataSize;
			BufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(
				CmdBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				0,
				0,
				nullptr,
				1,
				&BufferMemoryBarrier,
				0,
				nullptr
			);
		
			RenderContext->ExecuteTransientCommandBuffer(CmdBuffer);
		}
		else 
		{
			FVulkanMemoryAllocator* Allocator = FVulkanMemoryAllocator::Get();
			void* memory = Allocator->MapMemory(Allocation);
			memcpy((uint8*)memory + Offset, InData, DataSize);
			Allocator->UnmapMemory(Allocation);
		}
		
		if (Specification.Flags & (uint8)EDeviceBufferFlags::CREATE_STAGING_BUFFER)
		{
			//...
		}
		
		if (Specification.BufferUsage == EDeviceBufferUsage::INDEX_BUFFER) 
		{
			
			FIndexBufferData* IboData = static_cast<FIndexBufferData*>(InData);
			AssertMsg(IboData, "Invalid Index Buffer!");

			
			uint8 IndexSize = 1;
			if (Specification.Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT8)  IndexSize = 1;
			if (Specification.Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT16) IndexSize = 2;
			if (Specification.Flags & (uint64)EDeviceBufferFlags::INDEX_TYPE_UINT32) IndexSize = 4;
			
			IboData->IndexCount = DataSize / IndexSize;
			IboData->IndexType = ExtractIndexType(Specification.Flags);
		
		}
		
		else if (Specification.BufferUsage == EDeviceBufferUsage::VERTEX_BUFFER)
		{
			FVertexBufferData* VBOData = static_cast<FVertexBufferData*>(InData);
			AssertMsg(VBOData, "Invalid Vertex Buffer!");
			
			VBOData->VertexCount = DataSize / sizeof(float);
		}
	}
	
	uint64 FVulkanBuffer::GetAlignedSizeForBuffer(uint64 Size, EDeviceBufferUsage Usage)
	{
		VkPhysicalDeviceProperties DeviceProps;
		FVulkanRenderContext* RenderContext = FRenderer::GetRenderContext<FVulkanRenderContext>();		
	
		
		RenderContext->GetPhysicalDeviceProperties(DeviceProps);
    
		uint64 MinAlignment = 1;  // Default alignment
    
		// Only care about uniform and dynamic uniform buffer
		if (Usage == EDeviceBufferUsage::UNIFORM_BUFFER || Usage == EDeviceBufferUsage::UNIFORM_BUFFER_DYNAMIC)
		{
			MinAlignment = DeviceProps.limits.minUniformBufferOffsetAlignment;
		}
		if(Usage == EDeviceBufferUsage::STORAGE_BUFFER || Usage == EDeviceBufferUsage::STORAGE_BUFFER_DYNAMIC)
		{
			MinAlignment = DeviceProps.limits.minStorageBufferOffsetAlignment;
		}

		return Math::GetAligned(Size, MinAlignment);
	}

}
