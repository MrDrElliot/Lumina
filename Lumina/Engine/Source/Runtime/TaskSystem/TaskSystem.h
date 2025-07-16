#pragma once
#include "TaskScheduler.h"
#include "Core/Assertions/Assert.h"
#include "Core/Singleton/Singleton.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{

    using ITaskSet =            enki::ITaskSet;
    using IPinnedTask =         enki::IPinnedTask;
    using ICompletableTask =    enki::ICompletable;
    using AsyncTask =           enki::TaskSet;
    using TaskSetPartition =    enki::TaskSetPartition;
    using TaskFunction =        enki::TaskSetFunction;

    template<typename TLambda>
    class FLambdaTaskSet : public enki::ITaskSet
    {
    public:
        
        FLambdaTaskSet(TLambda&& InLambda)
            : Lambda(Memory::Move(InLambda)) {}

        void ExecuteRange(enki::TaskSetPartition range, uint32_t threadnum) override
        {
            Lambda();
        }

    private:
        TLambda Lambda;
    };

    template<typename TLambda>
    class FLambdaParallelTaskSet : public enki::ITaskSet
    {
    public:
        FLambdaParallelTaskSet(uint32 NumElements, TLambda&& InLambda)
            : Count(NumElements), Lambda(Memory::Move(InLambda)) {}

        void ExecuteRange(enki::TaskSetPartition range, uint32 threadnum) override
        {
            for (uint32 i = range.start; i < range.end; ++i)
                Lambda(i);
        }

    private:
        uint32 Count;
        TLambda Lambda;
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

        template<typename TLambda>
        void ScheduleLambda(TLambda&& Lambda)
        {
            auto* Task = new FLambdaTaskSet<std::decay_t<TLambda>>(Memory::Move(Lambda));
            ScheduleTask(Task);
        }

        template<typename Iterable, typename TLambda>
        void ParallelForEach(const Iterable& Range, TLambda&& Func)
        {
            using std::begin;
            using std::end;

            auto BeginIt = begin(Range);
            auto EndIt = end(Range);

            const uint32_t Count = static_cast<uint32_t>(std::distance(BeginIt, EndIt));
    
            if (Count == 0)
                return;

            auto TaskLambda = [BeginIt, Func = std::forward<TLambda>(Func)](uint32_t i)
            {
                auto It = BeginIt;
                std::advance(It, i);
                Func(i, *It);
            };

            this->ParallelFor(Count, TaskLambda);
        }
        
        template<typename TLambda>
        void ParallelFor(uint32 Count, TLambda&& Lambda)
        {
            auto* Task = new FLambdaParallelTaskSet<std::decay_t<TLambda>>(Count, Memory::Move(Lambda));
            ScheduleTask(Task);
            WaitForTask(Task);
            delete Task;
        }

        void ScheduleTask(ITaskSet* pTask)
        {
            Assert(bInitialized)
            Scheduler.AddTaskSetToPipe(pTask);
        }

        void ScheduleTask(IPinnedTask* pTask)
        {
            Assert(bInitialized)
            Scheduler.AddPinnedTask(pTask);
        }

        void WaitForTask(ITaskSet* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }

        void WaitForTask(IPinnedTask* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }


    private:

        enki::TaskScheduler     Scheduler;
        uint32                  NumWorkers = 0;
        bool                    bInitialized = false;
        std::atomic<uint32_t>   ActiveTasks = 0;

    };
}
