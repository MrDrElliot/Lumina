#pragma once
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Memory/Memory.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class IRenderContext;
}

namespace Lumina
{
    enum class ERHIAccess : uint32;
}

namespace Lumina
{
    class ICommandList;
    class IRenderPass;
    
    class FRenderGraphScope
    {
        friend class FRenderGraph;
    public:

        /** Throws an exception if not found */
        template<typename T>
        FRHIResourceRef Get()
        {
            uint64 HashCode = typeid(T).hash_code();
            return Resources.at(HashCode);
        }

        IRenderContext* GetRenderContext() const { return RenderContext; }
        ICommandList* GetCommandList() const { return CommandList; }

    private:
        
        IRenderContext*                     RenderContext = nullptr;
        ICommandList*                       CommandList = nullptr;
        THashMap<uint64, FRHIResourceRef>   Resources;
    };

    class FRenderGraphBuilder
    {
    public:

        template<typename T>
        void Write(ERHIAccess Access)
        {
            Writes.emplace_back(eastl::make_pair(typeid(T).hash_code(), Access));
        }

        template<typename T>
        void Read(ERHIAccess Access)
        {
            Reads.emplace_back(eastl::make_pair(typeid(T).hash_code(), Access));
        }
        
        TVector<TPair<uint64, ERHIAccess>> Reads;
        TVector<TPair<uint64, ERHIAccess>> Writes;
    };
    
    //=========================================================================================
    
    class FRenderGraph
    {
    public:

        FRenderGraph() = delete;
        FRenderGraph(IRenderContext* InContext)
            : RenderContext(InContext)
        {}
        
        void ClearPasses();
        void Execute(ICommandList* CommandList);
        void Compile();

        IRenderContext* GetRenderContext() const { return RenderContext; }
        
        template<typename T>
        void AddResource(IRHIResource* Resource)
        {
            uint64 HashID = typeid(T).hash_code();
            if (Resources.find(HashID) == Resources.end())
            {
                Resources.try_emplace(HashID, Resource);
            }
        }
        
        template<typename T>
        requires eastl::is_base_of_v<IRenderPass, T>
        void AddRenderPass(const FName& Name, TFunction<void(FRenderGraphBuilder&)>&& BuildCallback)
        {
            T* NewPass = Memory::New<T>();
            NewPass->PassName = Name;
            NewPass->BuildFunctor = Memory::Move(BuildCallback);
            AddPassInternal(NewPass);
        }


    private:

        void AddPassInternal(IRenderPass* InPass);

    private:

        IRenderContext*                     RenderContext;
        THashMap<uint64, FRHIResourceRef>   Resources;
        TVector<IRenderPass*>               RenderPasses;
    };
}
