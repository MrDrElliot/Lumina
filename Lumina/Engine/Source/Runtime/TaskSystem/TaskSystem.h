#pragma once

#include "TaskScheduler.h"
#include "TaskTypes.h"
#include "Core/Singleton/Singleton.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
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

            FLambdaTask* Task = nullptr;
            {
                FScopeLock Lock(LambdaTaskMutex);
                if (!LambdaTaskPool.empty() )
                {
                    if (LambdaTaskPool.back()->GetIsComplete())
                    {
                        Task = LambdaTaskPool.back();
                    }
                    
                    LambdaTaskPool.pop();
                }
            }

            if (!Task)
            {
                Task = Memory::New<FLambdaTask>();
            }

            Task->Reset(Priority, Num, Memory::Move(Function));
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
                    {
                        Func(i);
                    }
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

        void PushLambdaTaskToPool(FLambdaTask* InTask)
        {
            FScopeLock Lock(LambdaTaskMutex);
            LambdaTaskPool.push(InTask);
        }
    

    private:

        FMutex                  LambdaTaskMutex;
        TQueue<FLambdaTask*>    LambdaTaskPool;
        
        enki::TaskScheduler     Scheduler;
        uint32                  NumWorkers = 0;
        bool                    bInitialized = false;

    };
}
