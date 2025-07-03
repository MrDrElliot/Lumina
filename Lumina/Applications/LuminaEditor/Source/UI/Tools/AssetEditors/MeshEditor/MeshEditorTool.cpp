#include "MeshEditorTool.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Components/StaicMeshComponent.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Tools/UI/ImGui/ImGuiX.h"


namespace Lumina
{
    const char* MeshPropertiesName        = "MeshProperties";

    FMeshEditorTool::FMeshEditorTool(IEditorToolContext* Context, CObject* InAsset)
    : FAssetEditorTool(Context, InAsset->GetName().c_str(), InAsset)
    {
        FSceneManager* SceneManager = GEngine->GetEngineSubsystem<FSceneManager>();
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->RegisterSystem(Memory::New<FDebugCameraEntitySystem>());
        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
        
        FStaticMeshComponent& NewComponent = MeshEntity.AddComponent<FStaticMeshComponent>();
        NewComponent.StaticMesh = Cast<CStaticMesh>(InAsset);
        
        Scene = NewScene;
    }

    void FMeshEditorTool::OnInitialize()
    {
        CreateToolWindow(MeshPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CStaticMesh* StaticMesh = Cast<CStaticMesh>(Asset);

            ImGui::Text("Verticies: %u", StaticMesh->GetNumVertices());
            ImGui::Text("Indicies: %u", StaticMesh->GetNumIndicies());

            ImGuiX::DrawObjectProperties(StaticMesh);
        });
    }

    void FMeshEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FMeshEditorTool::OnAssetLoadFinished()
    {
    }

    void FMeshEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        FAssetEditorTool::DrawToolMenu(UpdateContext);
    }

    void FMeshEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGuiID leftDockID = 0, rightDockID = 0, bottomDockID = 0;

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Right, 0.3f, &rightDockID, &leftDockID);

        ImGui::DockBuilderSplitNode(InDockspaceID, ImGuiDir_Down, 0.3f, &bottomDockID, &InDockspaceID);

        ImGui::DockBuilderDockWindow(GetToolWindowName(ViewportWindowName).c_str(), leftDockID);
        ImGui::DockBuilderDockWindow(GetToolWindowName(MeshPropertiesName).c_str(), rightDockID);
    }
}
