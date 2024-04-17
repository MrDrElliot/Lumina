#pragma once
#include <memory>

#include "Image.h"

namespace Lumina
{
    class FCommandBuffer;
    struct FRenderConfig;
    class FRenderAPI
    {
    public:
        
        virtual ~FRenderAPI() = default;

        static FRenderAPI* Create(const FRenderConfig& InConfig);

    
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRender(std::shared_ptr<FImage> Target, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor) = 0;
        virtual void EndRender() = 0;
        virtual void WaitDevice() = 0;

        virtual std::shared_ptr<FImage> GetSwapchainImage() = 0;

        virtual void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value) = 0;

        virtual std::shared_ptr<FCommandBuffer> GetCommandBuffer() = 0;
        virtual void BeginCommandRecord() = 0;
        virtual void EndCommandRecord() = 0;
        virtual void ExecuteCurrentCommands() = 0;

        
    };
}
