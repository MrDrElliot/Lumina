#pragma once

#include "Scene.h"
#include "ScenePrimitives.h"
#include "SceneRenderTypes.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    class FPrimitiveDrawManager;
    struct FMaterialTexturesData;
    class FSceneUpdateContext;
    class AMaterialInstance;
    class FScene;
    class Material;
    class FRenderer;
    class FImageSampler;
    class AStaticMesh;
    class FBuffer;
    class FCamera;
    
    struct FModelData
    {
        glm::mat4 ModelMatrix;
    };
    
    /**
     * Scene renderer's are stateful renderes that interface with the state-less renderer.
     * Responsible for managing all low-level scene rendering.
     */
    class FSceneRenderer
    {
    public:
        

        static FSceneRenderer* Create();

        FSceneRenderer();
        virtual ~FSceneRenderer();

        void Initialize();
        void Deinitialize();
        
        void StartScene(const FScene* Scene);
        void EndScene(const FScene* Scene);
    
    protected:

        void ForwardRenderPass(const FScene* Scene);
        void FullScreenPass(const FScene* Scene);

        void DrawPrimitives(const FScene* Scene);

        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();

        FORCEINLINE FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

        
    private:

        FRHIViewportRef                     SceneViewport;

        FSceneGlobalData                    SceneGlobalData;
        FSceneLightData                     SceneLightData;
        TVector<FModelData>                 ModelData;
        TVector<FMaterialTexturesData>      TexturesData;
    };
    
}
