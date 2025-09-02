#include "TaskTypes.h"

#include "TaskSystem.h"
#include "Memory/Memory.h"

namespace Lumina
{
    void CompletionActionDelete::OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_)
    {
        // Call base class OnDependenciesComplete BEFORE deleting dependent task or self
        enki::ICompletable::OnDependenciesComplete( pTaskScheduler_, threadNum_ );
            
        // In this example we delete the dependency, which is safe to do as the task
        // manager will not dereference it at this point.
        // However, the dependency task should have no other dependents,
        // This class can have dependencies.
        Memory::Delete(Dependency.GetDependencyTask()); // also deletes this as member
    }

    void AsyncActionRecycle::OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_)
    {
        // Call base class OnDependenciesComplete BEFORE deleting dependent task or self
        enki::ICompletable::OnDependenciesComplete( pTaskScheduler_, threadNum_ );

        FTaskSystem::Get().PushLambdaTaskToPool((FLambdaTask*)Dependency.GetDependencyTask());
            
    }
}
