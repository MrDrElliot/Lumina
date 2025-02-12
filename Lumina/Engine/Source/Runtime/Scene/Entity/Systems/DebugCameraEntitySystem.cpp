#include "DebugCameraEntitySystem.h"

#include "Events/MouseCodes.h"
#include "Input/Input.h"
#include "Log/Log.h"
#include "Scene/SceneUpdateContext.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Entity/Components/CameraComponent.h"

namespace Lumina
{
    void FDebugCameraEntitySystem::Initialize(const FSubsystemManager* SubsystemManager)
    {
        
    }

    void FDebugCameraEntitySystem::Shutdown()
    {
        
    }

    void FDebugCameraEntitySystem::Update(entt::registry& EntityRegistry, const FSceneUpdateContext& UpdateContext)
    {
        double DeltaTime = UpdateContext.GetDeltaTime();
        float Speed = 5.0f; // Adjust movement speed

        for (auto CameraEntity : EntityRegistry.view<FCameraComponent, FTransformComponent>())
        {
            FTransformComponent& Transform = EntityRegistry.get<FTransformComponent>(CameraEntity);
            FCameraComponent& Camera = EntityRegistry.get<FCameraComponent>(CameraEntity);

            // Assuming Transform.Position is a glm::vec3
            glm::vec3 Forward = Transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 Right = Transform.Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);

            if (Input::IsKeyPressed(Key::W)) // Move Forward
            {
                Transform.Transform.Location += Forward * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::S)) // Move Backward
            {
                Transform.Transform.Location -= Forward * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::A)) // Move Left
            {
                Transform.Transform.Location -= Right * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::D)) // Move Right
            {
                Transform.Transform.Location += Right * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::Q)) // Move Down (world space)
            {
                Transform.Transform.Location += glm::vec3(0.0f, -1.0f, 0.0f) * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::E)) // Move Up (world space)
            {
                Transform.Transform.Location += glm::vec3(0.0f, 1.0f, 0.0f) * Speed * (float)DeltaTime;
            }

            if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
            {
                glm::vec2 CurrentMousePos = Input::GetMousePos();

                if (bFirstMove)
                {
                    PreviousMousePos = CurrentMousePos;
                    bFirstMove = false;
                }
    
                glm::vec2 MouseDelta = CurrentMousePos - PreviousMousePos;

                PreviousMousePos = CurrentMousePos;

                constexpr float Sensitivity = 0.1f;
                float YawDelta   = -MouseDelta.x * Sensitivity;
                float PitchDelta = -MouseDelta.y * Sensitivity;

                glm::quat yawQuat = glm::angleAxis(glm::radians(YawDelta), glm::vec3(0.0f, 1.0f, 0.0f));

                glm::vec3 RightAxis = Transform.Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
                glm::quat pitchQuat = glm::angleAxis(glm::radians(PitchDelta), RightAxis);

                Transform.Transform.Rotation = yawQuat * pitchQuat * Transform.Transform.Rotation;
            }
            else
            {
                bFirstMove = true;
            }
            
            glm::vec3 updatedForward = Transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 updatedUp = Transform.Transform.Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    
            Camera.SetView(Transform.Transform.Location, Transform.Transform.Location + updatedForward, updatedUp);            
        }
    }

}
