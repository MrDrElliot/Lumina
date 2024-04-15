#include "Scene.h"

namespace Lumina
{
    LScene::LScene() :LAsset(EAssetType::Scene)
    {
        EditorCamera = std::make_unique<FCamera>();
    }

    LScene::~LScene()
    {
    }

    void LScene::OnUpdate(float DeltaTime)
    {
        EditorCamera->Update(DeltaTime);
    }
}
