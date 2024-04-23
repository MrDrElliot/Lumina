#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "ScenePrimitives.h"

namespace Lumina
{
    class FBuffer;
    class FImage;
    class FCamera;

    class FSceneRenderer
    {
    public:

        static std::shared_ptr<FSceneRenderer> Create();

        FSceneRenderer();
        ~FSceneRenderer();

        void BeginScene(std::shared_ptr<FCamera> InCamera);
        void EndScene();

        void InitPipelines();
        void CreateImages();


    private:
        glm::mat4 viewMatrix;
        std::shared_ptr<FImage> ColorImage;
        std::shared_ptr<FImage> DepthImage;
        
        std::shared_ptr<FBuffer> VBO;
        std::shared_ptr<FBuffer> IBO;
        std::shared_ptr<FBuffer> CameraBuffer;
    
    };
}
