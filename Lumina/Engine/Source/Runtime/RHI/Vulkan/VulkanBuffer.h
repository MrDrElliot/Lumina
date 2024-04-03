#pragma once


#include <vector>
#include <vulkan/vulkan.h>


namespace Lumina
{
	struct FVulkanBuffer
	{
		VkDevice Device;
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory Memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo Descriptor;
		VkDeviceSize Size = 0;
		VkDeviceSize Alignment = 0;

		void* Mapped = nullptr;

		VkBufferUsageFlags UsageFlags;
		VkMemoryPropertyFlags MemoryPropertyFlags;

		/* Map a memory range of this buffer. If successful, mapped points to the specified buffer range. */
		VkResult Map(VkDeviceSize InSize = VK_WHOLE_SIZE, VkDeviceSize InOffset = 0);

		/* Unmap a mapped memory range */
		void Unmap();

		/* Attach the allocated memory block to the buffer */
		VkResult Bind(VkDeviceSize InOffset = 0);

		/* Setup the default descriptor for this buffer */
		void SetupDescriptor(VkDeviceSize InSize = VK_WHOLE_SIZE, VkDeviceSize InOffset = 0);

		/* Copies the specified data to the mapped buffer */
		void CopyTo(void* InData, VkDeviceSize InSize);
		
		/* Flush a memory range of the buffer to make it visible to the device */
		VkResult Flush(VkDeviceSize InSize = VK_WHOLE_SIZE, VkDeviceSize InOffset = 0);

		/* Invalidate a memory range of the buffer to make it visible to the host */
		VkResult Invalidate(VkDeviceSize InSize = VK_WHOLE_SIZE, VkDeviceSize InOffset = 0);

		/* Release all Vulkan resources held by this buffer */
		void Destroy();
	};
}