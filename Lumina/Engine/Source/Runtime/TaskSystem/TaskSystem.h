#pragma once
#include "LockLessMultiReadPipe.h"
#include "TaskScheduler.h"
#include "Core/Assertions/Assert.h"
#include "Core/Singleton/Singleton.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{

    using ITaskSet =            enki::ITaskSet;
    using IPinnedTask =         enki::IPinnedTask;
    using ICompleteableTask =   enki::ICompletable;
    using AsyncTask =           enki::TaskSet;
    using TaskSetPartition =    enki::TaskSetPartition;
    using TaskFunction =        enki::TaskSetFunction;

    
    class FTaskSystem : public TSingleton<FTaskSystem>
    {
    public:

        
        FORCEINLINE bool IsBusy() const { return Scheduler.GetIsShutdownRequested(); }
        FORCEINLINE uint32_t GetNumWorkers() const { return NumWorkers; }
        FORCEINLINE bool IsInitialized() const { return bInitialized; }

        void Initialize();
        void Shutdown();
        
        FORCEINLINE void WaitForAll() 
        {
            Scheduler.WaitforAll(); 
        }

        FORCEINLINE void ScheduleTask(ITaskSet* pTask)
        {
            Assert(bInitialized)
            Scheduler.AddTaskSetToPipe(pTask);
        }

        FORCEINLINE void ScheduleTask(IPinnedTask* pTask)
        {
            Assert(bInitialized)
            Scheduler.AddPinnedTask(pTask);
        }

        FORCEINLINE void WaitForTask(ITaskSet* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }

        FORCEINLINE void WaitForTask(IPinnedTask* pTask)
        {
            Scheduler.WaitforTask(pTask);
        }


    private:

        enki::TaskScheduler     Scheduler;
        uint32                  NumWorkers = 0;
        bool                    bInitialized = false;
    };
}
