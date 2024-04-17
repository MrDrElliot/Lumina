#pragma once
#include <functional>
#include <memory>
#include <glm/glm.hpp>

#include "CommandBuffer.h"

namespace Lumina
{
    class FImage;
    class FRenderAPI;
    class FWindow;
    class FRenderContext;

    struct FRenderConfig
    {
        int FramesInFlight;
        FWindow* Window;
    };
    
    class FRenderer
    {
    public:
        
        using RenderFunction = std::function<void(std::shared_ptr<FCommandBuffer> CmdBuffer)>;

        static void Init(const FRenderConfig& InConfig);
        static void Shutdown();

        static void Submit(RenderFunction Functor);

        
        static void BeginFrame();
        static void EndFrame();
        static void BeginRender(std::shared_ptr<FImage> Target, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor);
        static void EndRender();
        static std::shared_ptr<FImage> GetSwapchainImage();

        static void Render();
        static void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value);

        static std::shared_ptr<FCommandBuffer> GetCommandBuffer();
        
    private:

        static FRenderAPI* RenderAPI;
    };
    
}
