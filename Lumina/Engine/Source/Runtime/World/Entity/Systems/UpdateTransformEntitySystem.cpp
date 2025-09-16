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
        
        Task::ParallelFor((uint32)View.size(), [&](uint32 Index)
        {
            entt::entity entity = View->data()[Index];
            auto& transform = View.get<STransformComponent>(entity);
            
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
