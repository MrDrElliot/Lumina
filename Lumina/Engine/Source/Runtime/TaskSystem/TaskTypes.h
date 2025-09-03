#pragma once

#include "TaskScheduler.h"
#include "Core/Functional/Function.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    using ITaskSet =            enki::ITaskSet;
    using IPinnedTask =         enki::IPinnedTask;
    using ICompletableTask =    enki::ICompletable;
    using TaskSetPartition =    enki::TaskSetPartition;
    using TaskFunction =        enki::TaskSetFunction;

    enum class ETaskPriority
    {
        High   = 0,
        Medium = 1,
        Low    = 2,
    };
    
    struct CompletionActionDelete : enki::ICompletable
    {
        enki::Dependency    Dependency;
    
        // We override OnDependenciesComplete to provide an 'action' which occurs after
        // the dependency task is complete.
        void OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_ ) override;
    };

    struct AsyncActionRecycle : enki::ICompletable
    {
        enki::Dependency    Dependency;

        // We override OnDependenciesComplete to provide an 'action' which occurs after
        // the dependency task is complete.
        void OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_ ) override;
    };

    typedef TFunction<void (uint32 Start, uint32 End, uint32 Thread)> TaskSetFunction;
    class FLambdaTask : public ITaskSet
    {
    public:

        FLambdaTask(ETaskPriority Priority, uint32 SetSize, TaskSetFunction TaskFunctor)
        {
            m_Priority = (enki::TaskPriority)Priority;
            m_SetSize = SetSize;
            Function = Memory::Move(TaskFunctor);
            TaskRecycle.SetDependency(TaskRecycle.Dependency, this);
        }
        
        FLambdaTask()
        {
            TaskRecycle.SetDependency(TaskRecycle.Dependency, this);
        }
        
        void ExecuteRange(TaskSetPartition range_, uint32_t threadnum_) override
        {
            Function(range_.start, range_.end, threadnum_);
        }

        void Reset(ETaskPriority Priority, uint32 SetSize, TaskSetFunction TaskFunctor)
        {
            m_Priority = (enki::TaskPriority)Priority;
            m_SetSize = SetSize;
            Function = Memory::Move(TaskFunctor);
        }

        TaskSetFunction             Function;
        AsyncActionRecycle          TaskRecycle;
    };
    
}
