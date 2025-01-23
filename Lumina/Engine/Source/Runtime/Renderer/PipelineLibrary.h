#pragma once

#include <shared_mutex>
#include "Pipeline.h"
#include "Core/Singleton/Singleton.h"

namespace Lumina
{

    class FPipelineLibrary : public TSingleton<FPipelineLibrary>
    {
    public:

        
        static TRefPtr<FPipeline> GetPipeline(const FPipelineSpecification& spec)
        {
            for (auto& pipeline : Get()->Pipelines)
            {
                if (pipeline->GetSpecification() == spec)
                {
                    return pipeline;
                }
            }
            return nullptr;
        }

        static TRefPtr<FPipeline> GetPipelineByTag(const LString& Tag)
        {
            for (TRefPtr<FPipeline>& Pipeline : Get()->Pipelines)
            {
                if(Pipeline->GetSpecification().DebugName == Tag)
                {
                    return Pipeline;
                }
            }
            return nullptr;
        }

        static bool HasPipeline(const FPipelineSpecification& spec)
        {
            for (TRefPtr<FPipeline>& pipeline : Get()->Pipelines)
            {
                if (pipeline->GetSpecification() == spec)
                    return true;
            }
            return false;
        }

        static void AddPipeline(const TRefPtr<FPipeline>& pipeline)
        {
            if (pipeline != nullptr)
            {
                std::lock_guard lock(Get()->Mutex);
                Get()->Pipelines.push_back(pipeline); 
            }
        }

        void Shutdown() override
        {
            for (auto Pipeline : Get()->Pipelines)
            {
                Pipeline->Destroy();
            }
        }

    private:

        
        TFastVector<TRefPtr<FPipeline>> Pipelines;
        std::shared_mutex Mutex;

    };
}
