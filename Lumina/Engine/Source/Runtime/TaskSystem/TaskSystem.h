#pragma once

#include "TaskScheduler.h"
#include "TaskTypes.h"
#include "Core/Singleton/Singleton.h"
#include "Core/Threading/Thread.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    LUMINA_API extern class FTaskSystem* GTaskSystem;

    class FTaskSystem
    {
    public:
        
        bool IsBusy() const { return Scheduler.GetIsShutdownRequested(); }
        uint32_t GetNumWorkers() const { return NumWorkers; }

        static void Initialize();
        static void Shutdown();
        
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
         * Task::AsyncTask(10, [](uint32 Start, uint32 End, uint32 Thread)
         * {
         *      for(uint32 i = Start; i < End; ++i)
         *      {
         *          //.... [i] will be randomly distributed.
         *      }
         * });
         *
         * 
         * @param Num Number of executions.
         * @param Function Callback
         * @param Priority 
         * @return The task you can wait on, but should not be saved as it will be cleaned up automatically.
         */
        LUMINA_API FLambdaTask* ScheduleLambda(uint32 Num, TaskSetFunction&& Function, ETaskPriority Priority = ETaskPriority::Medium)
        {
            if (Num == 0)
            {
                LOG_WARN("Task Size of [0] passed to task system.");
                return nullptr;
            }

            FLambdaTask* Task = nullptr;
            if (!LambdaTaskPool.empty())
            {
                if (LambdaTaskPool.back()->GetIsComplete())
                {
                    Task = LambdaTaskPool.back();
                }
                
                LambdaTaskPool.pop();
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
        
        LUMINA_API void ScheduleTask(ITaskSet* pTask)
        {
            Scheduler.AddTaskSetToPipe(pTask);
        }

        LUMINA_API void ScheduleTask(IPinnedTask* pTask)
        {
            Scheduler.AddPinnedTask(pTask);
        }

        LUMINA_API void WaitForTask(const ITaskSet* pTask, ETaskPriority Priority = ETaskPriority::Low)
        {
            Scheduler.WaitforTask(pTask, (enki::TaskPriority)Priority);
        }

        LUMINA_API void WaitForTask(const IPinnedTask* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }

        void PushLambdaTaskToPool(FLambdaTask* InTask)
        {
            LambdaTaskPool.push(InTask);
        }
    

    private:

        FMutex                  LambdaTaskMutex;
        TQueue<FLambdaTask*>    LambdaTaskPool;
        
        enki::TaskScheduler     Scheduler;
        uint32                  NumWorkers = 0;
    };


    namespace Task
    {
        LUMINA_API FLambdaTask* AsyncTask(uint32 Num, TaskSetFunction&& Function, ETaskPriority Priority = ETaskPriority::Medium);

        template<typename TFunc>
        inline void ParallelFor(uint32 Num, TFunc&& Func, ETaskPriority Priority = ETaskPriority::Medium)
        {
            GTaskSystem->ParallelFor(Num, std::forward<TFunc>(Func), Priority);
        }
    }
}
