#include "VulkanDescriptorCache.h"

#include "VulkanRenderContext.h"
#include "VulkanResources.h"
#include "Core/Math/Hash/Hash.h"
#include "Core/Profiler/Profile.h"

namespace Lumina
{
    FRHIBindingLayoutRef FVulkanDescriptorCache::GetOrCreateLayout(FVulkanDevice* Device, const FBindingLayoutDesc& Desc)
    {
        LUMINA_PROFILE_SCOPE();
        SIZE_T Hash = Hash::GetHash(Desc);

        if (LayoutMap.find(Hash) != LayoutMap.end())
        {
            return LayoutMap.at(Hash);
        }

        FRHIBindingLayoutRef Layout = MakeRefCount<FVulkanBindingLayout>(Device, Desc);
        LayoutMap.try_emplace(Hash, Layout);

        return Layout;
    }
    

    FRHIBindingSetRef FVulkanDescriptorCache::GetOrCreateSet(FVulkanRenderContext* RenderContext, const FBindingSetDesc& Desc, FRHIBindingLayout* InLayout)
    {
        LUMINA_PROFILE_SCOPE();
        SIZE_T Hash = Hash::GetHash(Desc);

        if (SetMap.find(Hash) != SetMap.end())
        {
            return SetMap.at(Hash);
        }

        FRHIBindingSetRef Set = MakeRefCount<FVulkanBindingSet>(RenderContext, RenderContext->GetDevice(), Desc, static_cast<FVulkanBindingLayout*>(InLayout));
        SetMap.try_emplace(Hash, Set);

        return Set;

    }

    void FVulkanDescriptorCache::ReleaseResources()
    {
        LayoutMap.clear();
        SetMap.clear();
    }
}
