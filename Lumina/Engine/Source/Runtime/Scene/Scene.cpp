#include "Scene.h"

#include "Renderer/Material.h"
#include "SceneRenderer.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "Components/NameComponent.h"
#include "Entity/Entity.h"

namespace Lumina
{
    LScene::LScene(): LAsset(FAssetMetadata())
    {
        EditorCamera = std::make_shared<FCamera>();
        SceneRenderer = FSceneRenderer::Create(this);
        
        FMaterialTextures Textures;
        
        TFastVector<uint8> pixels;
        pixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Textures/Default_albedo.jpg");
        
        // Configure the texture specifications
        FImageSpecification TextureSpec = FImageSpecification::Default();
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_UNORM;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = std::move(pixels);
        TextureSpec.DebugName = "BaseColor";
        
        /* Base Color */
        Textures.BaseColor = FImage::Create(TextureSpec);

        TFastVector<uint8> normalpixels;
        normalpixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Textures/Default_normal.jpg");

        
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_SRGB;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = std::move(normalpixels);
        TextureSpec.DebugName = "BaseColor";

        Textures.Normal = FImage::Create(TextureSpec);

        TFastVector<uint8> rougnesspixels;
        rougnesspixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Textures/Default_metalRoughness.jpg");

        
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_UNORM;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = std::move(rougnesspixels);
        TextureSpec.DebugName = "BaseColor";

        Textures.MetallicRoughness = FImage::Create(TextureSpec);

        TFastVector<uint8> emissivePixels;
        emissivePixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Textures/Default_emissive.jpg");

        
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_UNORM;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = std::move(emissivePixels);
        TextureSpec.DebugName = "BaseColor";

        Textures.Emissive = FImage::Create(TextureSpec);

        TFastVector<uint8> ambientPixels;
        ambientPixels = FTextureFactory::ImportFromSource("../Lumina/Engine/Resources/Textures/Default_AO.jpg");

        
        TextureSpec.Extent = {2048, 2048, 1};
        TextureSpec.Usage = EImageUsage::TEXTURE;
        TextureSpec.Format = EImageFormat::RGBA32_UNORM;
        TextureSpec.MipLevels = 1;
        TextureSpec.ArrayLayers = 1;
        TextureSpec.Pixels = ambientPixels;
        TextureSpec.DebugName = "BaseColor";

        Textures.AmbientOcclusion = FImage::Create(TextureSpec);
        FMaterialAttributes Attributes;
        
        //TestMaterial = Material::Create(FPipelineLibrary::GetPipelineByTag("GraphicsPipeline"), Textures, Attributes);
        
        TAssetHandle<LStaticMesh> Mesh = AssetRegistry::GetAssetByPath<LStaticMesh>("../Sandbox/Game/Content/Avacado.lum");
        FAssetMetadata Metadata = Mesh->GetAssetMetadata();
    }

    LScene::~LScene()
    {
        
    }

    void LScene::BeginScene()
    {
        SceneRenderer->BeginScene(EditorCamera);
    }

    void LScene::OnUpdate(double DeltaTime)
    {
        BeginScene();
        EndScene();
    }

    void LScene::EndScene()
    {
        SceneRenderer->EndScene();
    }
    
    void LScene::Shutdown()
    {
        SceneRenderer->Shutdown();
    }

    Entity LScene::CreateEntity(const FTransform& Transform, const LString& Name)
    {
        Entity NewEntity(mEntityRegistery.create(), this);
        NewEntity.AddComponent<TransformComponent>(Transform);
        NewEntity.AddComponent<NameComponent>(Name);

        return NewEntity;
    }

    void LScene::DestroyEntity(Entity Entity)
    {
        mEntityRegistery.destroy(Entity);
    }

    
}
