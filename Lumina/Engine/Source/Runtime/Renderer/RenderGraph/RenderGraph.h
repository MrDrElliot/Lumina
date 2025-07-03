#pragma once
#include "Containers/Array.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class ICommandList;
    class IRenderPass;

    class FRenderGraph
    {
    public:

        void ClearPasses();
        void Execute(ICommandList* CommandList);
        void Compile();
        
        
        template<typename T, typename... Args>
        requires eastl::is_base_of_v<IRenderPass, T>
        void AddRenderPass(Args&&... args)
        {
            T* NewPass = Memory::New<T>(std::forward<Args>(args)...);
            AddPassInternal(NewPass);
        }


    private:

        void AddPassInternal(IRenderPass* InPass);

    private:

        TVector<IRenderPass*> RenderPasses;
    };
}
