#include "Scene.h"

#include "SceneRenderer.h"

namespace Lumina
{
    LScene::LScene(): LAsset(FAssetMetadata())
    {
        EditorCamera = std::make_shared<FCamera>();
        SceneRenderer = std::make_shared<FSceneRenderer>();
    }

    LScene::~LScene()
    {
    }

    void LScene::OnUpdate(double DeltaTime)
    {
        
        SceneRenderer->BeginScene(EditorCamera);

        SceneRenderer->EndScene();
    }

    void LScene::Shutdown()
    {
        SceneRenderer->Shutdown();
    }
}
