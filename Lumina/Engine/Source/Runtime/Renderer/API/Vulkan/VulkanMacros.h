#pragma once

#include "Platform/Platform.h"
#include "Log/Log.h"
#include <vulkan/vulkan.hpp>


namespace Lumina::Vulkan
{
    inline FString VkResultToString(VkResult result)
    {
        switch (result)
        {
            case VK_SUCCESS:                            return "VK_SUCCESS: Command successfully completed.";
            case VK_NOT_READY:                          return "VK_NOT_READY: A fence or query has not yet completed.";
            case VK_TIMEOUT:                            return "VK_TIMEOUT: A wait operation has not completed in the specified time.";
            case VK_EVENT_SET:                          return "VK_EVENT_SET: An event is signaled.";
            case VK_EVENT_RESET:                        return "VK_EVENT_RESET: An event is unsignaled.";
            case VK_INCOMPLETE:                         return "VK_INCOMPLETE: A return array was too small for the result.";
            case VK_ERROR_OUT_OF_HOST_MEMORY:           return "VK_ERROR_OUT_OF_HOST_MEMORY: A host memory allocation has failed.";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:         return "VK_ERROR_OUT_OF_DEVICE_MEMORY: A device memory allocation has failed.";
            case VK_ERROR_INITIALIZATION_FAILED:        return "VK_ERROR_INITIALIZATION_FAILED: Initialization of an object could not be completed for implementation-specific reasons.";
            case VK_ERROR_DEVICE_LOST:                  return "VK_ERROR_DEVICE_LOST: The logical or physical device has been lost.";
            case VK_ERROR_MEMORY_MAP_FAILED:            return "VK_ERROR_MEMORY_MAP_FAILED: Mapping of a memory object has failed.";
            case VK_ERROR_LAYER_NOT_PRESENT:            return "VK_ERROR_LAYER_NOT_PRESENT: A requested layer is not present or could not be loaded.";
            case VK_ERROR_EXTENSION_NOT_PRESENT:        return "VK_ERROR_EXTENSION_NOT_PRESENT: A requested extension is not supported.";
            case VK_ERROR_FEATURE_NOT_PRESENT:          return "VK_ERROR_FEATURE_NOT_PRESENT: A requested feature is not supported.";
            case VK_ERROR_INCOMPATIBLE_DRIVER:          return "VK_ERROR_INCOMPATIBLE_DRIVER: The requested version of Vulkan is not supported by the driver.";
            case VK_ERROR_TOO_MANY_OBJECTS:             return "VK_ERROR_TOO_MANY_OBJECTS: Too many objects of the type have already been created.";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:         return "VK_ERROR_FORMAT_NOT_SUPPORTED: A requested format is not supported on this device.";
            case VK_ERROR_FRAGMENTED_POOL:              return "VK_ERROR_FRAGMENTED_POOL: A pool allocation has failed due to fragmentation.";
            case VK_ERROR_SURFACE_LOST_KHR:             return "VK_ERROR_SURFACE_LOST_KHR: A surface is no longer available.";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:     return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: The requested window is already in use by Vulkan.";
            case VK_ERROR_OUT_OF_DATE_KHR:              return "VK_ERROR_OUT_OF_DATE_KHR: A surface has changed in such a way that it is no longer compatible with the swapchain.";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:     return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: The display used by a swapchain is incompatible.";
            case VK_ERROR_INVALID_SHADER_NV:            return "VK_ERROR_INVALID_SHADER_NV: One or more shaders failed to compile or link.";
            case VK_ERROR_OUT_OF_POOL_MEMORY:           return "VK_ERROR_OUT_OF_POOL_MEMORY: A pool memory allocation has failed.";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:      return "VK_ERROR_INVALID_EXTERNAL_HANDLE: An external handle is not valid.";
            case VK_ERROR_FRAGMENTATION:                return "VK_ERROR_FRAGMENTATION: A descriptor pool creation has failed due to fragmentation.";
            case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:   return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: A buffer creation failed because the requested address is not available.";
            default:                                    return "Unknown Vulkan error code.";
        }
    }
}

#define VULKAN_TIMEOUT_ONE_SECOND 1000000000
#define VK_NO_FLAGS 0

// Macro to check Vulkan function results and log errors if necessary
#ifdef LE_DEBUG
#define VK_CHECK(x)                                                                 \
do {                                                                                \
    VkResult result = (x);                                                          \
    if (UNLIKELY(result < 0))                                                       \
    {                                                                               \
        LOG_WARN(                                                                   \
            "Vulkan Error:\n"                                                       \
            "  Function Call : {0}\n"                                               \
            "  File          : {1}\n"                                               \
            "  Line          : {2}\n"                                               \
            "  Error Code    : {3} ({4})",                                          \
            #x, __FILE__, __LINE__, result, Vulkan::VkResultToString(result));      \
        if (result == VK_ERROR_DEVICE_LOST)                                         \
        {                                                                           \
            std::exit(static_cast<uint32>(result));                                 \
        }                                                                           \
    }                                                                               \
} while (0)

#else
#define VK_CHECK(x) x
#endif