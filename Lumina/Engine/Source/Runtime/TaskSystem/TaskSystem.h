#pragma once
#include "TaskScheduler.h"
#include "Core/Assertions/Assert.h"
#include "Core/Functional/Function.h"
#include "Core/Singleton/Singleton.h"
#include "Memory/Memory.h"
#include "Memory/SmartPtr.h"
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
        void OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_ ) override
        {
            // Call base class OnDependenciesComplete BEFORE deleting dependent task or self
            enki::ICompletable::OnDependenciesComplete( pTaskScheduler_, threadNum_ );
            
            // In this example we delete the dependency, which is safe to do as the task
            // manager will not dereference it at this point.
            // However, the dependency task should have no other dependents,
            // This class can have dependencies.
            Memory::Delete(Dependency.GetDependencyTask()); // also deletes this as member
        }
    };
    

    typedef TFunction<void (uint32 Start, uint32 End, uint32 Thread)> TaskSetFunction;
    class FLambdaTask : public ITaskSet
    {
    public:
        FLambdaTask() = default;
        FLambdaTask(TaskSetFunction func_)
            : Function(std::move(func_))
        {
            TaskDeleter.SetDependency(TaskDeleter.Dependency, this);
        }
        FLambdaTask(uint32 setSize_, TaskSetFunction func_)
            : ITaskSet(setSize_), Function(std::move(func_))
        {
            TaskDeleter.SetDependency(TaskDeleter.Dependency, this);
        }

        void ExecuteRange(TaskSetPartition range_, uint32_t threadnum_) override
        {
            Function(range_.start, range_.end, threadnum_);
        }
        
        TaskSetFunction             Function;
        CompletionActionDelete      TaskDeleter;
        enki::Dependency            Dependency;
    };

    typedef TFunction<void (uint32 Index)> FParallelForTaskFunction;
    class FParallelForTask : public ITaskSet
    {
    public:
        FParallelForTask() = default;
        FParallelForTask(FParallelForTaskFunction func_)
            : Function(std::move(func_))
        {
            TaskDeleter.SetDependency(TaskDeleter.Dependency, this);
        }
        
        FParallelForTask(uint32 setSize_, FParallelForTaskFunction func_)
            : ITaskSet(setSize_), Function(std::move(func_))
        {
            TaskDeleter.SetDependency(TaskDeleter.Dependency, this);
        }

        void ExecuteRange(TaskSetPartition range_, uint32_t threadnum_) override
        {
            for (uint32 i = range_.start; i < range_.end; ++i)
            {
                Function(i);
            }
        }
        
        FParallelForTaskFunction    Function;
        CompletionActionDelete      TaskDeleter;
        enki::Dependency            Dependency;
    };
    
    class LUMINA_API FTaskSystem : public TSingleton<FTaskSystem>
    {
    public:

        
        bool IsBusy() const { return Scheduler.GetIsShutdownRequested(); }
        uint32_t GetNumWorkers() const { return NumWorkers; }
        bool IsInitialized() const { return bInitialized; }

        void Initialize();
        void Shutdown();
        
        void WaitForAll() 
        {
            Scheduler.WaitforAll(); 
        }

        /**
         * When scheduling tasks, the number specified is the number of iterations you want. EnkiTS will -
         * divide up the tasks between the available threads, it's important to note that these are *NOT*
         * executed in order. The ranges will be random, but they will all be executed only once, but if you -
         * need the index to be consistent. This will not work for you. Here's an example of a parallel for loop.
         *
         * FTaskSystem::Get()->ScheduleLambda(10, [](uint32 Start, uint32 End, uint32 Thread)
         * {
         *      for(uint32 i = Start; i < End; ++i)
         *      {
         *          //.... i will be randomly distributed.
         *      }
         * });
         *
         * 
         * @param Num Number of executions.
         * @param Function Callback
         * @param Priority 
         * @return The task you can wait on, but should not be saved as it will be cleaned up automatically.
         */
        FLambdaTask* ScheduleLambda(uint32 Num, TaskSetFunction&& Function, ETaskPriority Priority = ETaskPriority::Medium)
        {
            if (Num == 0)
            {
                return nullptr;
            }
            
            FLambdaTask* Task = Memory::New<FLambdaTask>(Num, std::move(Function));
            Task->m_Priority = (enki::TaskPriority)Priority;
            ScheduleTask(Task);
            return Task;
        }
        
        template<typename TFunc>
        void ParallelFor(uint32 Num, TFunc&& Func, ETaskPriority Priority = ETaskPriority::Medium)
        {
            struct ParallelTask : ITaskSet
            {
                ParallelTask(TFunc&& InFunc, uint32 InNum)
                    : Func(std::forward<TFunc>(InFunc))
                {
                    m_SetSize = InNum;
                }

                void ExecuteRange(TaskSetPartition range_, uint32_t threadnum_) override
                {
                    for (uint32 i = range_.start; i < range_.end; ++i)
                        Func(i);
                }

                TFunc Func;
            };

            ParallelTask Task = ParallelTask(std::forward<TFunc>(Func), Num);
            Task.m_Priority = (enki::TaskPriority)Priority;
            ScheduleTask(&Task);
            WaitForTask(&Task);
        }
        
        void ScheduleTask(ITaskSet* pTask)
        {
            Scheduler.AddTaskSetToPipe(pTask);
        }

        void ScheduleTask(IPinnedTask* pTask)
        {
            Scheduler.AddPinnedTask(pTask);
        }

        void WaitForTask(const ITaskSet* pTask, ETaskPriority Priority = ETaskPriority::Low)
        {
            Scheduler.WaitforTask(pTask, (enki::TaskPriority)Priority);
        }

        void WaitForTask(const IPinnedTask* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }


    private:

        enki::TaskScheduler     Scheduler;
        uint32                  NumWorkers = 0;
        bool                    bInitialized = false;

    };
}
