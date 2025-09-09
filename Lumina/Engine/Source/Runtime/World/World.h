#pragma once

#include "Core/Object/Object.h"
#include "Core/UpdateContext.h"
#include "World/Entity/Components/CameraComponent.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Entity/Registry/EntityRegistry.h"
#include "World.generated.h"

namespace Lumina
{
    struct SRenderComponent;
    class FCameraManager;
    struct FLineBatcherComponent;
    class FSceneRenderer;
    class CEntitySystem;
    class Entity;
}

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CWorld : public CObject
    {
        GENERATED_BODY()
        friend class FWorldManager;
    public:

        CWorld();

        //~ Begin CObject Interface
        void Serialize(FArchive& Ar) override;
        //~ End CObject Interface


        /**
         * Initializes systems and renderer.
         */
        void InitializeWorld();
        
        /** Handles setting up this world for editor use,
         * returns the entity used during world manipulation
         * */
        Entity SetupEditorWorld();


        /**
         * Called on every update stage and runs systems attached to this world.
         */
        void Update(const FUpdateContext& Context);

        /**
         * Called to shut down the world, destroys system, components, and entities.
         */
        void ShutdownWorld();

        bool RegisterSystem(CEntitySystem* NewSystem);

        Entity ConstructEntity(const FName& Name, const FTransform& Transform = FTransform());
        
        void CopyEntity(Entity& To, const Entity& From);
        void ReparentEntity(Entity Child, Entity Parent);
        void DestroyEntity(Entity Entity);
        
        FEntityRegistry& GetMutableEntityRegistry() { return EntityRegistry; }
        const FEntityRegistry& GetConstEntityRegistry() const { return EntityRegistry; }

        void SetActiveCamera(Entity InEntity);
        SCameraComponent& GetActiveCamera() const;

        double GetSceneDeltaTime() const { return DeltaTime; }
        double GetTimeSinceSceneCreation() const { return TimeSinceCreation; }

        void SetPaused(bool bNewPause) { bPaused = bNewPause; }
        bool IsPaused() const { return bPaused; }

        void SetActive(bool bNewActive) { bActive = bNewActive; }
        bool IsSuspended() const { return !bActive; }

        static CWorld* DuplicateWorldForPIE(CWorld* OwningWorld);

        FSceneRenderer* GetRenderer() const { return SceneRenderer; }

        const TVector<TObjectHandle<CEntitySystem>>& GetSystemsForUpdateStage(EUpdateStage Stage);

        //~ Begin Debug Drawing
        void DrawDebugLine(const glm::vec3& Start, const glm::vec3& End, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f);
        void DrawDebugBox(const glm::vec3& Center, const glm::vec3& Extents, const glm::quat& Rotation, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f);
        void DrawDebugSphere(const glm::vec3& Center, float Radius, const glm::vec4& Color, uint8 Segments = 16, float Thickness = 1.0f, float Duration = 1.0f);
        void DrawDebugCone(const glm::vec3& Apex, const glm::vec3& Direction, float AngleRadians, float Length, const glm::vec4& Color, uint8 Segments = 16, uint8 Stacks = 4, float Thickness = 1.0f, float Duration = 1.0f);
        void DrawFrustum(const glm::mat4& Matrix, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f);
        void DrawArrow(const glm::vec3& Start, const glm::vec3& Direction, float Length, const glm::vec4& Color, float Thickness = 1.0f, float Duration = 1.0f, float HeadSize = 0.2f);
        //~ End Debug Drawing

        void SetIsPlayInEditorWorld(bool bValue) { bIsPlayInEditorWorld = bValue; }
        
    private:
        
        FLineBatcherComponent& GetOrCreateLineBatcher();
    
    private:

        FLineBatcherComponent*                          LineBatcherComponent = nullptr;

        FCameraManager*                                 CameraManager = nullptr;
        FSceneRenderer*                                 SceneRenderer = nullptr;
        FEntityRegistry                                 EntityRegistry;
        
        TVector<TObjectHandle<CEntitySystem>>           SystemUpdateList[(int32)EUpdateStage::Max];

    private:

        int64                                           WorldIndex = -1;
        double                                          DeltaTime = 0.0;
        double                                          TimeSinceCreation = 0.0;
        uint32                                          bPaused:1=1;
        uint32                                          bActive:1=1;
        uint32                                          bIsPlayInEditorWorld:1=0;
        
    };
}
