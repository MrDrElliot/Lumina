#include "MeshEditorTool.h"

#include "ImGuiDrawUtils.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Cast.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "glm/gtc/type_ptr.inl"
#include "Scene/SceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Scene/SceneRenderTypes.h"
#include "Scene/Entity/Components/LightComponent.h"
#include "Scene/Entity/Components/StaticMeshComponent.h"
#include "Scene/Entity/Components/VelocityComponent.h"
#include "Scene/Entity/Systems/DebugCameraEntitySystem.h"
#include "Tools/UI/ImGui/ImGuiFonts.h"
#include "Tools/UI/ImGui/ImGuiX.h"


namespace Lumina
{
    static const char* MeshPropertiesName        = "MeshProperties";

    FMeshEditorTool::FMeshEditorTool(IEditorToolContext* Context, CObject* InAsset)
    : FAssetEditorTool(Context, InAsset->GetName().c_str(), InAsset)
    {
        FSceneManager* SceneManager = GEngine->GetEngineSubsystem<FSceneManager>();
        FScene* NewScene = SceneManager->CreateScene(ESceneType::Tool);
        NewScene->RegisterSystem(NewObject<CDebugCameraEntitySystem>());

        Entity DirectionalLightEntity = NewScene->CreateEntity(FTransform(), "Directional Light");
        DirectionalLightEntity.AddComponent<SDirectionalLightComponent>();
        
        MeshEntity = NewScene->CreateEntity(FTransform(), "MeshEntity");
        
        MeshEntity.AddComponent<SStaticMeshComponent>().StaticMesh = Cast<CStaticMesh>(InAsset);
        MeshEntity.GetComponent<STransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, -2.5f));
        
        Scene = NewScene;
    }

    void FMeshEditorTool::OnInitialize()
    {
        CreateToolWindow(MeshPropertiesName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            CStaticMesh* StaticMesh = Cast<CStaticMesh>(Asset.Get());

            const FMeshResource& Resource = StaticMesh->GetMeshResource();
            TVector<FGeometrySurface> Surfaces = Resource.GeometrySurfaces;
            
            if (ImGui::CollapsingHeader("Mesh Resources"))
            {
                ImGui::Indent();

                if (ImGui::BeginTable("MeshResourceTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    auto Row = [](const char* label, const FString& value)
                    {
                           ImGui::TableNextRow();
                           ImGui::TableSetColumnIndex(0);
                           ImGui::TextUnformatted(label);
                           ImGui::TableSetColumnIndex(1);
                           ImGui::TextUnformatted(value.c_str());
                    };

                    Row("Vertices", eastl::to_string(Resource.Vertices.size()));
                    Row("Indices", eastl::to_string(Resource.Indices.size()));
                    Row("Surfaces", eastl::to_string(Resource.GetNumSurfaces()));

                    ImGui::EndTable();
                }

                ImGui::Unindent();
            }

            ImGuiX::Font::PushFont(ImGuiX::Font::EFont::Large);
            ImGui::SeparatorText("Geometry Surfaces");
            ImGuiX::Font::PopFont();
            
            for (size_t i = 0; i < Resource.GeometrySurfaces.size(); ++i)
            {
                const FGeometrySurface& Surface = Resource.GeometrySurfaces[i];
                ImGui::PushID(i);
                ImGui::Text("Name: %s", Surface.ID.c_str());
                ImGui::Text("Material Index: %lld", Surface.MaterialIndex);
                ImGui::Text("Index Count: %u", Surface.IndexCount);
                ImGui::Text("Start Index: %llu", Surface.StartIndex);

                ImGui::Separator();
                ImGui::PopID();
            }

            ImGui::SeparatorText("Asset Details");

            PropertyTable.DrawTree();
            
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

        if (ImGui::BeginMenu(LE_ICON_CAMERA_CONTROL" Camera Control"))
        {
            float Speed = EditorEntity.GetComponent<SVelocityComponent>().Speed;
            ImGui::SliderFloat("Camera Speed", &Speed, 1.0f, 200.0f);
            EditorEntity.GetComponent<SVelocityComponent>().Speed = Speed;
            ImGui::EndMenu();
        }
        
        // Gizmo Control Dropdown
        if (ImGui::BeginMenu(LE_ICON_MOVE_RESIZE " Gizmo Control"))
        {
            const char* operations[] = { "Translate", "Rotate", "Scale" };
            static int currentOp = 0;

            if (ImGui::Combo("##", &currentOp, operations, IM_ARRAYSIZE(operations)))
            {
                switch (currentOp)
                {
                case 0: GuizmoOp = ImGuizmo::TRANSLATE; break;
                case 1: GuizmoOp = ImGuizmo::ROTATE;    break;
                case 2: GuizmoOp = ImGuizmo::SCALE;     break;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(LE_ICON_DEBUG_STEP_INTO " Render Debug"))
        {
            FSceneRenderer* SceneRenderer = UpdateContext.GetSubsystem<FSceneManager>()->GetSceneRendererForScene(Scene);
            const FSceneRenderStats& Stats = SceneRenderer->GetSceneRenderStats();

            ImGui::TextColored(ImVec4(1.0f, 0.78f, 0.16f, 1.0f), "Scene Statistics");
            ImGui::Separator();
            ImGui::Text("Draw Calls");    ImGui::SameLine(150); ImGui::Text("%u", Stats.NumDrawCalls);
            ImGui::Text("Vertices");      ImGui::SameLine(150); ImGui::Text("%llu", Stats.NumVertices);
            ImGui::Text("Indices");       ImGui::SameLine(150); ImGui::Text("%llu", Stats.NumIndices);
            ImGui::Text("Mesh Proxies");  ImGui::SameLine(150); ImGui::Text("%llu", SceneRenderer->GetNumMeshProxies());

            ImGui::Spacing();
    
            ImGui::TextColored(ImVec4(0.58f, 0.86f, 1.0f, 1.0f), "Debug Visualization");
            ImGui::Separator();

            static const char* GBufferDebugLabels[] =
            {
                "RenderTarget",
                "Albedo",
                "Position",
                "Normals",
                "Material"
            };

            ESceneRenderGBuffer DebugMode = SceneRenderer->GetGBufferDebugMode();
            int DebugModeInt = static_cast<int>(DebugMode);
            ImGui::PushItemWidth(200);
            if (ImGui::Combo("GBuffer Mode", &DebugModeInt, GBufferDebugLabels, IM_ARRAYSIZE(GBufferDebugLabels)))
            {
                SceneRenderer->SetGBufferDebugMode(static_cast<ESceneRenderGBuffer>(DebugModeInt));
            }
            ImGui::PopItemWidth();

            ImGui::EndMenu();
        }
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
