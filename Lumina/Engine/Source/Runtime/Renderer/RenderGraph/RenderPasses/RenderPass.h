#pragma once
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "EASTL/variant.h"
#include "Memory/RefCounted.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    enum class ERenderPassAccess : uint8
    {
        Read,
        Write,
    };
    
    class IRenderPass
    {
    public:

        IRenderPass(const IRenderPass&) = delete;
        IRenderPass() = delete;
        IRenderPass(const FName& InName)
            : PassName(InName)
        {}

        FName GetPassName() const { return PassName; }

        virtual void Execute(ICommandList* CommandList) = 0;

    private:
        
        FName                               PassName;
    };
}
