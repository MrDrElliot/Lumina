#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

#include "Source/Runtime/Log/Log.h"


namespace Lumina
{
    class FThread
    {
    public:
        FThread(const std::string& ThreadName)
            : ShouldStop(false), Name(ThreadName) {}

        virtual ~FThread()
        {
            RequestStop();
            WorkerThread.join();
        }

        FThread(const FThread&) = delete;
        FThread& operator=(const FThread&) = delete;

        void Start()
        {
            WorkerThread = std::thread(&FThread::ProcessTasks, this);
        }

        // Use an ID to ensure task uniqueness
        template<typename T>
        void Submit(T&& Task, const std::string& TaskId)
        {
            std::unique_lock<std::mutex> lock(Mutex);
            // Check if task ID already exists to prevent duplicates
            if (Tasks.find(TaskId) == Tasks.end())
            {
                Tasks.emplace(TaskId, std::forward<T>(Task));
            }
        }

        void Dispatch()
        {
            std::unique_lock<std::mutex> lock(Mutex);
            ShouldDispatch = true;
            Condition.notify_one();
        }

        void RequestStop()
        {
            std::unique_lock<std::mutex> lock(Mutex);
            ShouldStop = true;
            Condition.notify_all();
        }

        void Join()
        {
            if (WorkerThread.joinable())
            {
                WorkerThread.join();
            }
        }

        std::thread::id GetThreadID() const
        {
            return WorkerThread.get_id();
        }
        
        static std::thread::id GetCurrentThreadID()
        {
            return std::this_thread::get_id();
        }

    protected:
        void ProcessTasks()
        {
            while (!ShouldStop)
            {
                std::unique_lock<std::mutex> lock(Mutex);
                Condition.wait(lock, [this] { return ShouldDispatch || ShouldStop; });

                if (ShouldStop) break;

                for (auto& [id, task] : Tasks)
                {
                    task();
                }

                ShouldDispatch = false; // Reset the flag to wait for the next Dispatch call
            }
        }

        std::atomic<bool> ShouldStop;
        std::atomic<bool> ShouldDispatch{false};
        std::thread WorkerThread;
        std::string Name;
        std::map<std::string, std::function<void()>> Tasks; // Map to ensure unique tasks
        std::mutex Mutex;
        std::condition_variable Condition;
    };
}
