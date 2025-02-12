#include "RendererInfoEditorTool.h"

#include "Renderer/Renderer.h"
#include "Renderer/RHI/Vulkan/VulkanRenderContext.h"

namespace Lumina
{
void FRendererInfoEditorTool::OnInitialize(const FUpdateContext& UpdateContext)
{
        CreateToolWindow("Vulkan Device Info", [this](const FUpdateContext& Context, bool bFocused)
        {
            auto VulkanContext = FRenderer::GetRenderContext<FVulkanRenderContext>();
    
            const VkPhysicalDeviceProperties& DeviceProps = VulkanContext->GetPhysicalDeviceProperties();
            const VkPhysicalDeviceMemoryProperties& MemProps = VulkanContext->GetPhysicalDeviceMemoryProperties();
            const VkPhysicalDeviceFeatures& DeviceFeatures = VulkanContext->GetPhysicalDeviceFeatures();
    
            // General Device Info
            ImGui::Text("GPU Name: %s", DeviceProps.deviceName);
            ImGui::Separator();
            
            ImGui::Text("API Version: %d.%d.%d",
                VK_VERSION_MAJOR(DeviceProps.apiVersion),
                VK_VERSION_MINOR(DeviceProps.apiVersion),
                VK_VERSION_PATCH(DeviceProps.apiVersion));
    
            ImGui::Text("Driver Version: %d", DeviceProps.driverVersion);
            ImGui::Text("Vendor ID: 0x%X", DeviceProps.vendorID);
            ImGui::Text("Device ID: 0x%X", DeviceProps.deviceID);
            ImGui::Text("Device Type: %s",
                (DeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ? "Discrete GPU" :
                (DeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ? "Integrated GPU" :
                (DeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) ? "Virtual GPU" :
                (DeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) ? "CPU" : "Unknown");
    
    
            // Resource Limits
            ImGui::SeparatorText("Device Limits");
            
            ImGui::Text("Max 2D Image Dimension: %u", DeviceProps.limits.maxImageDimension2D);
            ImGui::Text("Max 3D Image Dimension: %u", DeviceProps.limits.maxImageDimension3D);
            ImGui::Text("Max Cube Image Dimension: %u", DeviceProps.limits.maxImageDimensionCube);
            ImGui::Text("Max Uniform Buffer Range: %u", DeviceProps.limits.maxUniformBufferRange);
            ImGui::Text("Max Storage Buffer Range: %u", DeviceProps.limits.maxStorageBufferRange);
            ImGui::Text("Max Push Constant Size: %u", DeviceProps.limits.maxPushConstantsSize);
            ImGui::Text("Max Bound Descriptor Sets: %u", DeviceProps.limits.maxBoundDescriptorSets);
            ImGui::Text("Max Per-Stage Descriptor Samplers: %u", DeviceProps.limits.maxPerStageDescriptorSamplers);
            ImGui::Text("Max Per-Stage Descriptor Uniform Buffers: %u", DeviceProps.limits.maxPerStageDescriptorUniformBuffers);
            ImGui::Text("Max Per-Stage Descriptor Storage Buffers: %u", DeviceProps.limits.maxPerStageDescriptorStorageBuffers);
            ImGui::Text("Max Per-Stage Descriptor Sampled Images: %u", DeviceProps.limits.maxPerStageDescriptorSampledImages);
            ImGui::Text("Max Per-Stage Descriptor Storage Images: %u", DeviceProps.limits.maxPerStageDescriptorStorageImages);
            ImGui::Text("Max Per-Stage Descriptor Input Attachments: %u", DeviceProps.limits.maxPerStageDescriptorInputAttachments);
            ImGui::Text("Max Descriptor Set Samplers: %u", DeviceProps.limits.maxDescriptorSetSamplers);
            ImGui::Text("Max Descriptor Set Uniform Buffers: %u", DeviceProps.limits.maxDescriptorSetUniformBuffers);
            ImGui::Text("Max Descriptor Set Storage Buffers: %u", DeviceProps.limits.maxDescriptorSetStorageBuffers);
            ImGui::Text("Max Descriptor Set Sampled Images: %u", DeviceProps.limits.maxDescriptorSetSampledImages);
            ImGui::Text("Max Descriptor Set Storage Images: %u", DeviceProps.limits.maxDescriptorSetStorageImages);
            ImGui::Text("Max Descriptor Set Input Attachments: %u", DeviceProps.limits.maxDescriptorSetInputAttachments);
            ImGui::Text("Max Compute Shared Memory Size: %u", DeviceProps.limits.maxComputeSharedMemorySize);
            ImGui::Text("Max Compute Work Group Count: (%u, %u, %u)", 
                DeviceProps.limits.maxComputeWorkGroupCount[0],
                DeviceProps.limits.maxComputeWorkGroupCount[1],
                DeviceProps.limits.maxComputeWorkGroupCount[2]);
            ImGui::Text("Max Compute Work Group Invocations: %u", DeviceProps.limits.maxComputeWorkGroupInvocations);
    
            // Memory Heaps
            ImGui::SeparatorText("Device Memory Heaps");
            for (uint32_t i = 0; i < MemProps.memoryHeapCount; i++)
            {
                ImGui::Text("Heap %d: %llu MB (%s)", 
                    i, 
                    MemProps.memoryHeaps[i].size / (1024 * 1024),
                    (MemProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "Device Local" : "Host Visible");
            }
    
            ImGui::SeparatorText("Device Memory Types");
            for (uint32_t i = 0; i < MemProps.memoryTypeCount; i++)
            {
                ImGui::Text("Type %d: Heap %d, Flags: %s %s %s %s",
                    i, MemProps.memoryTypes[i].heapIndex,
                    (MemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ? "DeviceLocal" : "",
                    (MemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? "HostVisible" : "",
                    (MemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ? "HostCoherent" : "",
                    (MemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) ? "HostCached" : "");
            }
            
            // Supported Features
            ImGui::SeparatorText("Device Supported Features");
            ImGui::Text("Geometry Shader: %s", DeviceFeatures.geometryShader ? "Yes" : "No");
            ImGui::Text("Tessellation Shader: %s", DeviceFeatures.tessellationShader ? "Yes" : "No");
            ImGui::Text("Wide Lines: %s", DeviceFeatures.wideLines ? "Yes" : "No");
            ImGui::Text("Large Points: %s", DeviceFeatures.largePoints ? "Yes" : "No");
            ImGui::Text("Anisotropic Filtering: %s", DeviceFeatures.samplerAnisotropy ? "Yes" : "No");
            ImGui::Text("Multi-Viewport Rendering: %s", DeviceFeatures.multiViewport ? "Yes" : "No");
            ImGui::Text("Fill Mode Non-Solid: %s", DeviceFeatures.fillModeNonSolid ? "Yes" : "No");
            ImGui::Text("Shader Clip Distance: %s", DeviceFeatures.shaderClipDistance ? "Yes" : "No");
            ImGui::Text("Shader Cull Distance: %s", DeviceFeatures.shaderCullDistance ? "Yes" : "No");
            ImGui::Text("Independent Blend: %s", DeviceFeatures.independentBlend ? "Yes" : "No");
            ImGui::Text("Logic Operations: %s", DeviceFeatures.logicOp ? "Yes" : "No");
            ImGui::Text("Draw Indirect First Instance: %s", DeviceFeatures.drawIndirectFirstInstance ? "Yes" : "No");
        });
    }



    void FRendererInfoEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }
}
