#include "DebugCameraEntitySystem.h"
#include "Core/Application/Application.h"
#include "Events/MouseCodes.h"
#include "Input/Input.h"
#include "Input/InputSubsystem.h"
#include "World/Entity/Entity.h"
#include "World/Entity/Components/CameraComponent.h"
#include "World/Entity/Components/EditorComponent.h"
#include "World/Entity/Components/VelocityComponent.h"
#include "world/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
    void CDebugCameraEntitySystem::Initialize()
    {
        
    }

    void CDebugCameraEntitySystem::Shutdown()
    {
        
    }

    void CDebugCameraEntitySystem::Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext)
    {
        double DeltaTime = UpdateContext.GetDeltaTime();
        
        for (auto CameraEntity : EntityRegistry.view<SEditorComponent, SCameraComponent>())
        {
            STransformComponent& Transform = EntityRegistry.get<STransformComponent>(CameraEntity);
            SCameraComponent& Camera = EntityRegistry.get<SCameraComponent>(CameraEntity);
            SEditorComponent& Editor = EntityRegistry.get<SEditorComponent>(CameraEntity);
            SVelocityComponent& Velocity = EntityRegistry.get<SVelocityComponent>(CameraEntity);
    
            if (!Editor.bEnabled)
                continue;
    
            glm::vec3 Forward = Transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 Right   = Transform.Transform.Rotation * glm::vec3(1.0f, 0.0f,  0.0f);
            glm::vec3 Up      = Transform.Transform.Rotation * glm::vec3(0.0f, 1.0f,  0.0f);
    
            float Speed = Velocity.Speed;
            if (Input::IsKeyPressed(Key::LeftShift))
                Speed *= 5.0f;
    
            glm::vec3 Acceleration(0.0f);
    
            if (Input::IsKeyPressed(Key::W)) Acceleration += Forward;
            if (Input::IsKeyPressed(Key::S)) Acceleration -= Forward;
            if (Input::IsKeyPressed(Key::D)) Acceleration += Right;
            if (Input::IsKeyPressed(Key::A)) Acceleration -= Right;
            if (Input::IsKeyPressed(Key::E)) Acceleration += Up;
            if (Input::IsKeyPressed(Key::Q)) Acceleration -= Up;
    
            if (glm::length(Acceleration) > 0.0f)
                Acceleration = glm::normalize(Acceleration) * Speed;
    
            // Integrate acceleration to velocity
            Velocity.Velocity += Acceleration * (float)DeltaTime;
    
            // Apply simple linear drag
            constexpr float Drag = 10.0f;
            Velocity.Velocity -= Velocity.Velocity * Drag * (float)DeltaTime;
    
            // Apply velocity to position
            Transform.Transform.Location += Velocity.Velocity * (float)DeltaTime;
    
            // Mouse look
            if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
            {
                glfwSetInputMode(Windowing::GetPrimaryWindowHandle()->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
                float MousePitchDelta = UpdateContext.GetSubsystem<FInputSubsystem>()->GetMouseDeltaPitch();
                float MouseYawDelta   = UpdateContext.GetSubsystem<FInputSubsystem>()->GetMouseDeltaYaw();
    
                constexpr float Sensitivity = 0.1f;
                float YawDelta   = -MouseYawDelta   * Sensitivity;
                float PitchDelta = MousePitchDelta * Sensitivity;
    
                glm::quat yawQuat   = glm::angleAxis(glm::radians(YawDelta), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 RightAxis = Transform.Transform.Rotation * glm::vec3(-1.0f, 0.0f, 0.0f);
                glm::quat pitchQuat = glm::angleAxis(glm::radians(PitchDelta), RightAxis);
    
                Transform.Transform.Rotation = glm::normalize(yawQuat * pitchQuat * Transform.Transform.Rotation);
            }
            else
            {
                glfwSetInputMode(Windowing::GetPrimaryWindowHandle()->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
    
            // Update camera view
            glm::vec3 UpdatedForward = Transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 UpdatedUp      = Transform.Transform.Rotation * glm::vec3(0.0f, 1.0f,  0.0f);
    
            Camera.SetView(Transform.Transform.Location, Transform.Transform.Location + UpdatedForward, UpdatedUp);
        }
    }
}
