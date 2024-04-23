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

        void OnUpdate(float DeltaTime);

        std::shared_ptr<FCamera> GetEditorCamera() { return EditorCamera; }
        

    private:
        
        std::shared_ptr<FCamera> EditorCamera;
        std::unique_ptr<FSceneRenderer> SceneRenderer;
    };
}
