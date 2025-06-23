#include "DebugCameraEntitySystem.h"
#include "Lumina.h"
#include "Core/Application/Application.h"
#include "Events/MouseCodes.h"
#include "Input/Input.h"
#include "Input/InputSubsystem.h"
#include "Scene/SceneUpdateContext.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Entity/Components/EditorComponent.h"

namespace Lumina
{
    void FDebugCameraEntitySystem::Initialize(const FSubsystemManager* SubsystemManager)
    {
        
    }

    void FDebugCameraEntitySystem::Shutdown()
    {
        
    }

    void FDebugCameraEntitySystem::Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext)
    {
        double DeltaTime = UpdateContext.GetDeltaTime();
        float Speed = 5.0f;

        for (auto CameraEntity : EntityRegistry.view<FEditorComponent, FCameraComponent>())
        {
            FTransformComponent& TransformComponent =    EntityRegistry.get<FTransformComponent>(CameraEntity);
            FCameraComponent& CameraComponent = EntityRegistry.get<FCameraComponent>(CameraEntity);
            FEditorComponent& EditorComponent = EntityRegistry.get<FEditorComponent>(CameraEntity);


            if (EditorComponent.IsEnabled() == false)
            {
                continue;
            }
            
            glm::vec3 Forward = TransformComponent.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 Right = TransformComponent.Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);

            if (Input::IsKeyPressed(Key::LeftShift))
            {
                Speed *= 3.0f;
            }

            if (Input::IsKeyPressed(Key::W)) // Move Forward
            {
                TransformComponent.Transform.Location += Forward * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::S)) // Move Backward
            {
                TransformComponent.Transform.Location -= Forward * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::A)) // Move Left
            {
                TransformComponent.Transform.Location -= Right * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::D)) // Move Right
            {
                TransformComponent.Transform.Location += Right * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::Q)) // Move Down (world space)
            {
                TransformComponent.Transform.Location += glm::vec3(0.0f, -1.0f, 0.0f) * Speed * (float)DeltaTime;
            }

            if (Input::IsKeyPressed(Key::E)) // Move Up (world space)
            {
                TransformComponent.Transform.Location += glm::vec3(0.0f, 1.0f, 0.0f) * Speed * (float)DeltaTime;
            }

            if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
            {
                //@TODO Fixme, better interface for the application window without needing to access the application itself.
                glfwSetInputMode(FApplication::Get().GetMainWindow()->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                float MousePitchDelta = UpdateContext.GetSubsystem<FInputSubsystem>()->GetMouseDeltaPitch();
                float MouseYawDelta = UpdateContext.GetSubsystem<FInputSubsystem>()->GetMouseDeltaYaw();


                constexpr float Sensitivity = 0.1f;
                float YawDelta   = -MouseYawDelta * Sensitivity;
                float PitchDelta = -MousePitchDelta * Sensitivity;
                
                glm::quat yawQuat = glm::angleAxis(glm::radians(YawDelta), glm::vec3(0.0f, 1.0f, 0.0f));

                glm::vec3 RightAxis = TransformComponent.Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
                glm::quat pitchQuat = glm::angleAxis(glm::radians(PitchDelta), RightAxis);

                TransformComponent.Transform.Rotation = yawQuat * pitchQuat * TransformComponent.Transform.Rotation;
            }
            else
            {
                glfwSetInputMode(FApplication::Get().GetMainWindow()->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            
            glm::vec3 updatedForward = TransformComponent.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 updatedUp = TransformComponent.Transform.Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    
            CameraComponent.SetView(TransformComponent.Transform.Location, TransformComponent.Transform.Location + updatedForward, updatedUp);            
        }
    }

}
