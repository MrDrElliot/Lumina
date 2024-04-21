#include "VulkanBuffer.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanRenderContext.h"
#include "glm/glm.hpp"

namespace Lumina
{
	VkBufferUsageFlags convert(EDeviceBufferUsage Usage) 
	{
		switch (Usage)
		{
		case EDeviceBufferUsage::VERTEX_BUFFER:				    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case EDeviceBufferUsage::INDEX_BUFFER:					return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		case EDeviceBufferUsage::UNIFORM_BUFFER:				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		case EDeviceBufferUsage::STORAGE_BUFFER:				return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		case EDeviceBufferUsage::STAGING_BUFFER:				return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
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
    
	FVulkanBuffer::FVulkanBuffer(const FDeviceBufferSpecification& Spec): Buffer(VK_NULL_HANDLE), Specification(Spec), Data(nullptr)
	{
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		glm::uint64 VmaFlags = convert(Spec.MemoryUsage);

		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.size = Spec.Size;
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferCreateInfo.usage = convert(Spec.BufferUsage);

		Allocation = Alloc->AllocateBuffer(&BufferCreateInfo, VmaFlags, &Buffer);
	}

	FVulkanBuffer::FVulkanBuffer(const FDeviceBufferSpecification& Spec, void* Data, glm::uint64 DataSize)
	: Buffer(VK_NULL_HANDLE), Specification(Spec), Data(nullptr)
	{
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		glm::uint64 vma_flags = convert(Spec.MemoryUsage);

		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = Spec.Size;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_create_info.usage = convert(Spec.BufferUsage);

		if (Specification.MemoryUsage == EDeviceBufferMemoryUsage::NO_HOST_ACCESS)
		{
			buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		Allocation = Alloc->AllocateBuffer(&buffer_create_info, vma_flags, &Buffer);
		this->UploadData(0, Data, DataSize);
        
	}

	FDeviceBufferSpecification FVulkanBuffer::GetSpecification() const
	{
		return Specification;
	}

	void FVulkanBuffer::UploadData(glm::uint64 Offset, void* Data, glm::uint64 DataSize)
	{
		if (Specification.MemoryUsage == EDeviceBufferMemoryUsage::NO_HOST_ACCESS) 
		{
			FVulkanRenderContext& RenderContext = FVulkanRenderContext::Get();
		
			FDeviceBufferSpecification StagingBufferSpec = {};
			StagingBufferSpec.Size = DataSize;
			StagingBufferSpec.BufferUsage = EDeviceBufferUsage::STAGING_BUFFER;
			StagingBufferSpec.MemoryUsage = EDeviceBufferMemoryUsage::COHERENT_WRITE;
		
			FVulkanBuffer StagingBuffer(StagingBufferSpec, Data, DataSize);
		
			VkBufferCopy BufferCopy = {};
			BufferCopy.size = DataSize;
			BufferCopy.srcOffset = 0;
			BufferCopy.dstOffset = 0;
		
			VkCommandBuffer CmdBuffer = RenderContext.AllocateTransientCommandBuffer();
		
			vkCmdCopyBuffer(CmdBuffer, StagingBuffer.GetBuffer(), Buffer, 1, &BufferCopy);
		
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
		
			RenderContext.ExecuteTransientCommandBuffer(CmdBuffer);
		}
		else 
		{
			FVulkanMemoryAllocator* Allocator = FVulkanMemoryAllocator::Get();
			void* memory = Allocator->MapMemory(Allocation);
			memcpy((std::byte*)memory + Offset, Data, DataSize);
			//@ TODO Possible memory leak?
			Allocator->UnmapMemory(Allocation);
		}
		
		if (Specification.Flags & (glm::uint64)EDeviceBufferFlags::CREATE_STAGING_BUFFER)
			return;
		
		if (Specification.BufferUsage == EDeviceBufferUsage::INDEX_BUFFER) 
		{
			if(!Data) Data = new FIndexBufferData;
		
			FIndexBufferData* IboData = (FIndexBufferData*)Data;
			glm::uint8 IndexSize = 0;
			if (Specification.Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT8)  IndexSize = 1;
			if (Specification.Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT16) IndexSize = 2;
			if (Specification.Flags & (glm::uint64)EDeviceBufferFlags::INDEX_TYPE_UINT32) IndexSize = 4;
			
			IboData->IndexCount = DataSize / IndexSize;
			IboData->IndexType = ExtractIndexType(Specification.Flags);
		
		}
		
		else if (Specification.BufferUsage == EDeviceBufferUsage::VERTEX_BUFFER)
		{
			if (!Data) Data = new FVertexBufferData;
		
			FVertexBufferData* VBOData = (FVertexBufferData*)Data;
			
			VBOData->VertexCount = DataSize / sizeof(glm::float32);
		}
	
	}

	void FVulkanBuffer::Destroy()
	{
		FVulkanMemoryAllocator* Alloc = FVulkanMemoryAllocator::Get();
		Alloc->DestroyBuffer(&Buffer, &Allocation);

		if(Data) delete Data;
	}
}
