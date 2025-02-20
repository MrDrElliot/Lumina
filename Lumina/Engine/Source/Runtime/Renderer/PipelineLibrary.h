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
    
        TRefCountPtr<FPipeline> GetOrCreatePipeline(const FPipelineSpec& PipelineSpec);
        
        

    private:

        THashMap<uint64, TRefCountPtr<FPipeline>> PipelineMap;
    
    };
}
