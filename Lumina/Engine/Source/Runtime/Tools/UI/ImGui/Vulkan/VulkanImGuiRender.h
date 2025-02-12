#pragma once

#include <vulkan/vulkan_core.h>

#include "Scene/SceneRenderer.h"
#include "Tools/UI/ImGui/ImGuiRenderer.h"

namespace Lumina
{
    class FUpdateContext;

    class FVulkanImGuiRender: public IImGuiRenderer
    {
    public:

        FVulkanImGuiRender();
        ~FVulkanImGuiRender() override = default;

        void Initialize(const FSubsystemManager& Manager) override;
        void Deinitialize() override;
        
        void OnStartFrame() override;
        void OnEndFrame() override;

        void RenderImage(TRefPtr<FImage> Image, ImVec2 Size) override;
        void RenderImage(const ImGuiX::FImGuiImageInfo& ImageInfo) override;
        

        ImGuiX::FImGuiImageInfo CreateImGuiTexture(TRefPtr<FImage> Image, ImVec2 Size) override;
        ImGuiX::FImGuiImageInfo CreateImGuiTexture(const FString& RawPath) override;
        
        void DestroyImGuiTexture(const ImGuiX::FImGuiImageInfo& ImageInfo) override;
        void DestroyImGuiTexture(const TRefPtr<FImage>& Image) override;

    private:

        /** An ImTextureID in this context is castable to a VkDescriptorset. */
        void RegisterNewImage(FGuid Guid, ImTextureID NewImage, const FString& DebugName);
        
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
        
    };
    
}
