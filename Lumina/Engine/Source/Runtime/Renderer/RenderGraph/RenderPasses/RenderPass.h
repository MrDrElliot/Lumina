#pragma once

#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Renderer/RHIFwd.h"
#include "Scene/SceneRenderTypes.h"

namespace Lumina
{
    class IRenderContext;
}

namespace Lumina
{
    using FRenderPassFunction = TFunction<void()>;
    
    class FRenderPass
    {
    public:

        FRenderPass(const FName& Name)
            :PassName(Name)
        {}
        
        virtual ~FRenderPass() = default;
        
        virtual void SetPassFunc(const FRenderPassFunction& Function) { PassFunction = Function; }
        void Execute() { PassFunction(); }
        

        FName GetPassName() const { return PassName; }

        void AddProxy(FStaticMeshRender&& Proxy);
        void ClearProxies();

    protected:

        FRenderPassFunction PassFunction;
        FName PassName;
        FMutex ProxyMutex;
        TFixedVector<FStaticMeshRender, 100> RenderProxies;
        
    };
}
