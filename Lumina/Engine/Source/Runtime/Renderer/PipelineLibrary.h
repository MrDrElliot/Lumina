#pragma once
#include "Core/Singleton/Singleton.h"
#include "Lumina.h"
#include "Pipeline.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class AMaterial;
}

namespace Lumina
{
    class FPipelineLibrary : public TSingleton<FPipelineLibrary>
    {
        
    public:
    
        void Shutdown();    

        FRHIPipeline GetOrCreatePipeline(const FPipelineSpec& PipelineSpec);
        
    private:

        THashMap<uint64, FRHIPipeline> PipelineMap;
    
    };
}
