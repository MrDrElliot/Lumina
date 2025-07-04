#pragma once
#include "Containers/Array.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    struct FBindingLayoutDesc;
    class FVulkanDevice;
    struct FBindingSetDesc;
    class FVulkanRenderContext;
}

namespace Lumina
{
    class FVulkanDescriptorCache
    {
    public:

        FRHIBindingLayoutRef GetOrCreateLayout(FVulkanDevice* Device, const FBindingLayoutDesc& Desc);
        FRHIBindingSetRef GetOrCreateSet(FVulkanRenderContext* RenderContext, const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout);

        void ReleaseResources();
        
    private:

        THashMap<SIZE_T, FRHIBindingLayoutRef>  LayoutMap;
        THashMap<SIZE_T, FRHIBindingSetRef>     SetMap;
    };
}
