#pragma once
#include "Camera.h"
#include "Source/Runtime/Assets/Asset.h"


namespace Lumina
{
    class FSceneRenderer;
    class LScene : public LAsset
    {
    public:
        
        LScene();
        ~LScene();

        void OnUpdate(double DeltaTime);
        void Shutdown();

        std::shared_ptr<FCamera> GetEditorCamera() { return EditorCamera; }
        std::shared_ptr<FSceneRenderer> GetSceneRenderer() { return SceneRenderer; }
        

    private:
        
        std::shared_ptr<FCamera> EditorCamera;
        std::shared_ptr<FSceneRenderer> SceneRenderer;
    };
}
