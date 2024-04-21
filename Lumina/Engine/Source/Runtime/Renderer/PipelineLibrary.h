#pragma once
#include <memory>
#include <shared_mutex>

#include "Pipeline.h"

namespace Lumina
{

    class FPipelineLibrary
    {
    public:
        
        static std::shared_ptr<FPipeline> GetPipeline(const FPipelineSpecification& spec)
        {
            for (auto& pipeline : Pipelines)
            {
                if (pipeline->GetSpecification() == spec)
                {
                    return pipeline;
                }
            }
            return nullptr;
        }

        static std::shared_ptr<FPipeline> GetPipelineByTag(const std::string& Tag)
        {
            for (auto Pipeline : Pipelines)
            {
                if(Pipeline->GetSpecification().debug_name == Tag)
                {
                    return Pipeline;
                }
            }
            return nullptr;
        }

        static bool HasPipeline(const FPipelineSpecification& spec)
        {
            for (auto& pipeline : Pipelines)
            {
                if (pipeline->GetSpecification() == spec)
                    return true;
            }
            return false;
        }

        static void AddPipeline(std::shared_ptr<FPipeline> pipeline)
        { 
            std::lock_guard lock(Mutex);
            Pipelines.push_back(pipeline); 
        }

        static void Shutdown()
        {
            for (auto Pipeline : Pipelines)
            {
                Pipeline->Destroy();
            }
        }

    private:
        inline static std::vector<std::shared_ptr<FPipeline>> Pipelines;
        inline static std::shared_mutex Mutex;

    };
}
