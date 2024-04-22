#pragma once
#include <memory>

#include "Image.h"
#include "RenderTypes.h"

namespace Lumina
{
    class FSwapchain;
    class FDescriptorSet;
    class FBuffer;
    class FPipeline;
    class FCommandBuffer;
    struct FRenderConfig;
    class FRenderAPI
    {
    public:
        
        virtual ~FRenderAPI() = default;

        static FRenderAPI* Create(const FRenderConfig& InConfig);

    
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRender(const std::vector<std::shared_ptr<FImage>> Attachments, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor) = 0;
        virtual void EndRender() = 0;
        virtual void WaitDevice() = 0;

        virtual std::shared_ptr<FSwapchain> GetSwapchain() = 0;
        virtual std::shared_ptr<FImage> GetSwapchainImage() = 0;

        virtual void BindSet(std::shared_ptr<FDescriptorSet> Set, std::shared_ptr<FPipeline> Pipeline, glm::uint8 Index) = 0;
        virtual void BindPipeline(std::shared_ptr<FPipeline> Pipeline) = 0;
        
        virtual void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value) = 0;
        virtual void CopyToSwapchain(std::shared_ptr<FImage> ImageToCopy) = 0;

        
        virtual void RenderMeshTasks(std::shared_ptr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data) = 0;
        virtual void RenderMeshIndexed(std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer) = 0;
        virtual void RenderQuad(std::shared_ptr<FPipeline> Pipeline, FMiscData Data) = 0;
        virtual void RenderQuad(std::shared_ptr<FPipeline> Pipeline, glm::uint32 Amount, FMiscData Data) = 0;


        virtual std::shared_ptr<FCommandBuffer> GetCommandBuffer() = 0;
        virtual void BeginCommandRecord() = 0;
        virtual void EndCommandRecord() = 0;
        virtual void ExecuteCurrentCommands() = 0;

        virtual void RenderImGui() = 0;
        
    };
}
