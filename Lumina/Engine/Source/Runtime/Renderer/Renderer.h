#pragma once
#include <functional>
#include <memory>
#include <glm/glm.hpp>

#include "CommandBuffer.h"
#include "RenderTypes.h"


namespace Lumina
{
    class FDescriptorSet;
    class FBuffer;
    class FPipeline;
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
        
        using RenderFunction = std::function<void()>;

        static void Init(const FRenderConfig& InConfig);
        static void Shutdown();

        /* Don't forget to capture function inputs if needed */
        static void Submit(RenderFunction Functor);

        
        static void BeginFrame();
        static void EndFrame();
        static void BeginRender(const std::vector<std::shared_ptr<FImage>> Attachments, glm::uvec3 RenderArea, glm::ivec2 RenderOffset, glm::fvec4 ClearColor);
        static void EndRender();
        static void RenderImGui();
        static void Render();

        static void LoadShaderPack();

        static void BindSet(std::shared_ptr<FDescriptorSet> Set, std::shared_ptr<FPipeline> Pipeline, glm::uint8 Index);
        static void BindPipeline(std::shared_ptr<FPipeline> Pipeline);

        static std::shared_ptr<FImage> GetSwapchainImage();

        static void CopyToSwapchain(std::shared_ptr<FImage> ImageToCopy);
        static void ClearColor(std::shared_ptr<FImage> Image, const glm::fvec4& Value);

        static void RenderMeshTasks(std::shared_ptr<FPipeline> Pipeline, const glm::uvec3 Dimensions, FMiscData Data);
        static void RenderMeshIndexed(std::shared_ptr<FBuffer> VertexBuffer, std::shared_ptr<FBuffer> IndexBuffer);
        static void RenderQuad(std::shared_ptr<FPipeline> Pipeline, FMiscData Data);
        static void RenderQuad(std::shared_ptr<FPipeline> Pipeline, glm::uint32 Amount, FMiscData Data);

        
        static std::shared_ptr<FCommandBuffer> GetCommandBuffer();
        
    private:

        static FRenderAPI* RenderAPI;
    };
    
}
