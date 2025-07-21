#include "RenderPass.h"


namespace Lumina
{
    void FRenderPass::AddProxy(FMeshRenderProxy&& Proxy)
    {
        FScopeLock Lock(ProxyMutex);
        RenderProxies.push_back(Proxy);
    }

    void FRenderPass::ClearProxies()
    {
        FScopeLock Lock(ProxyMutex);
        RenderProxies.clear();
    }
}
