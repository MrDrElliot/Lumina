#include "Scene.h"

#include "SceneRenderer.h"

namespace Lumina
{
    LScene::LScene(): LAsset(EAssetType::StaticMesh, "")
    {
        EditorCamera = std::make_shared<FCamera>();
        SceneRenderer = std::make_unique<FSceneRenderer>();
    }

    LScene::~LScene()
    {
    }

    void LScene::OnUpdate(float DeltaTime)
    {
        
        SceneRenderer->BeginScene(EditorCamera);

        

        SceneRenderer->EndScene();
    }
}
