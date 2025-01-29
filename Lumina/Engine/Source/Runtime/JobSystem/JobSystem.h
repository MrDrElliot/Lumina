#pragma once

#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <future>
#include <optional>
#include <chrono>
#include "Core/Singleton/Singleton.h"


class FJobSystem : public TSingleton<FJobSystem>
{
public:
    explicit FJobSystem(size_t NumThreads = std::thread::hardware_concurrency())
        : bStop(false), ActiveThreads(0)
    {
        SetThreadPoolSize(NumThreads);
    }
    
    ~FJobSystem()
    {
        Shutdown();
    }

    template <typename Func>
    std::future<std::invoke_result_t<Func>> EnqueueTask(Func Task, int Priority = 0)
    {
        using ResultType = std::invoke_result_t<Func>;
        auto PackagedTask = MakeSharedPtr<std::packaged_task<ResultType()>>(std::move(Task));
        std::future<ResultType> Result = PackagedTask->get_future();

        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            TaskQueue.emplace(Priority, [PackagedTask]() { (*PackagedTask)(); });
        }

        CV.notify_one();
        return Result;
    }

    void SetThreadPoolSize(size_t NumThreads)
    {
        std::unique_lock<std::mutex> Lock(QueueMutex);

        size_t CurrentThreads = WorkerThreads.size();
        if (NumThreads > CurrentThreads)
        {
            // Add threads
            for (size_t i = 0; i < NumThreads - CurrentThreads; ++i)
            {
                WorkerThreads.emplace_back([this] { WorkerThreadLoop(); });
            }
        }
        else if (NumThreads < CurrentThreads)
        {
            // Reduce threads
            for (size_t i = 0; i < CurrentThreads - NumThreads; ++i)
            {
                EnqueueTask([this] { bStop = true; }, -1); // Low-priority stop task
            }
        }
    }

    size_t GetActiveThreads() const
    {
        return ActiveThreads.load();
    }

    size_t GetPendingTasks()
    {
        std::unique_lock<std::mutex> Lock(QueueMutex);
        return TaskQueue.size();
    }

    size_t GetNumWorkerThreads() const
    {
        return WorkerThreads.size();
    }

    void Shutdown()
    {
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            bStop = true;
        }

        CV.notify_all();

        for (auto& Thread : WorkerThreads)
        {
            if (Thread.joinable())
            {
                Thread.join();
            }
        }

        WorkerThreads.clear();
    }

private:
    using TaskType = std::pair<int, std::function<void()>>; // (priority, task)

    struct CompareTask
    {
        bool operator()(const TaskType& a, const TaskType& b) const
        {
            return a.first < b.first; // Higher priority first
        }
    };

    void WorkerThreadLoop()
    {
        while (true)
        {
            std::function<void()> Task;
            {
                std::unique_lock<std::mutex> Lock(QueueMutex);
                CV.wait(Lock, [this] { return bStop || !TaskQueue.empty(); });

                if (bStop && TaskQueue.empty())
                    return;

                Task = TaskQueue.top().second;
                TaskQueue.pop();

                ++ActiveThreads;
            }

            try
            {
                Task();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Task exception: " << e.what() << std::endl;
            }

            --ActiveThreads;
        }
    }

    std::vector<std::thread> WorkerThreads;
    std::priority_queue<TaskType, std::vector<TaskType>, CompareTask> TaskQueue;
    std::mutex QueueMutex;
    std::condition_variable CV;
    std::atomic<bool> bStop;
    std::atomic<size_t> ActiveThreads;
};
