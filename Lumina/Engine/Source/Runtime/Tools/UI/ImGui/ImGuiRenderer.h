#pragma once

#include "imgui.h"
#include "ImGuiX.h"
#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class FLayer;
    class FImageSampler;
    class FImage;
    
    class IImGuiRenderer : public ISubsystem
    {
    public:
        
        IImGuiRenderer() = default;
        virtual ~IImGuiRenderer() override = default;

        void Initialize(const FSubsystemManager& Manager) override;
        void Deinitialize() override { }
        
        void StartFrame();
        void EndFrame();
        
        virtual void OnStartFrame() = 0;
        virtual void OnEndFrame() = 0;
        
        virtual void RenderImage(TRefPtr<FImage> Image, ImVec2 Size) = 0;
        virtual void RenderImage(const ImGuiX::FImGuiImageInfo& ImageInfo) = 0;
        
        virtual ImGuiX::FImGuiImageInfo CreateImGuiTexture(TRefPtr<FImage> Image, ImVec2 Size) = 0;
        virtual ImGuiX::FImGuiImageInfo CreateImGuiTexture(const FString& RawPath) = 0;
        
        virtual void DestroyImGuiTexture(const ImGuiX::FImGuiImageInfo& ImageInfo) = 0;
        virtual void DestroyImGuiTexture(const TRefPtr<FImage>& Image) = 0;

    
    protected:

        THashMap<FGuid, ImGuiX::FImGuiImageInfo> ImageCache;
        
    };
}
