#include "UpdateTransformEntitySystem.h"

#include "Core/Object/Class.h"
#include "TaskSystem/TaskSystem.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Components/EditorComponent.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/Entity/Components/RenderComponent.h"
#include "World/Entity/Components/Transformcomponent.h"

namespace Lumina
{
    
    void CUpdateTransformEntitySystem::Initialize()
    {
        
    }

    void CUpdateTransformEntitySystem::Shutdown()
    {
        
    }

    void CUpdateTransformEntitySystem::Update(FSystemContext& SystemContext)
    {
        LUMINA_PROFILE_SCOPE();

        auto View = SystemContext.CreateView<STransformComponent>();
        auto RelationshipView = SystemContext.CreateView<SCameraComponent, SRelationshipComponent>();
        auto CameraView = SystemContext.CreateView<SCameraComponent>(entt::exclude<SEditorComponent>);
        
        Task::ParallelFor(View.size(), [&](uint32 Index)
        {
            entt::entity entity = View->data()[Index];
            auto& transform = View.get<STransformComponent>(entity);

            if (CameraView.contains(entity))
            {
                auto& Camera = CameraView.get<SCameraComponent>(entity);
                glm::vec3 UpdatedForward = transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 UpdatedUp      = transform.Transform.Rotation * glm::vec3(0.0f, 1.0f,  0.0f);
    
                Camera.SetView(transform.Transform.Location, transform.Transform.Location + UpdatedForward, UpdatedUp);
            }
            
            if (RelationshipView.contains(entity))
            {
                auto& relationship = RelationshipView.get<SRelationshipComponent>(entity);
            
                if (relationship.Parent.IsValid())
                {
                    transform.WorldTransform = relationship.Parent.GetComponent<STransformComponent>().WorldTransform * transform.Transform;
                }
                else
                {
                    transform.WorldTransform = transform.Transform;
                }
            }
            else
            {
                transform.WorldTransform = transform.Transform;
            }
            
            transform.CachedMatrix = transform.WorldTransform.GetMatrix();
        });
    }
}
