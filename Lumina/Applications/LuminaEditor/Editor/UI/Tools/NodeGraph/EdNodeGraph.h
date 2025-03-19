#pragma once
#include "EdGraphNode.h"
#include "imnodes_internal.h"
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Core/Templates/Forward.h"
#include "Memory/Memory.h"


namespace Lumina
{
    class FEdGraphNode;
}

namespace Lumina
{
    
    class FEdNodeGraph
    {
    public:

        struct FAction
        {
            FString             ActionName;
            TFunction<void()>   ActionCallback;
        };

        struct FNodeFactory
        {
            FString Name;
            FString Tooltip;
            TFunction<FEdGraphNode*()> CreationCallback;
        };
        

        FEdNodeGraph();
        virtual ~FEdNodeGraph();


        void DrawGraph();
        virtual void OnDrawGraph();
        
        void RegisterGraphAction(const FString& ActionName, const TFunction<void()>& ActionCallback);

        
        template<typename T, typename... Args>
        requires(std::is_base_of_v<FEdGraphNode, T> && std::is_constructible_v<T, Args...>)
        T* CreateNode(Args&&... args);

        template<typename T, typename LAMBDA>
        requires(std::is_invocable_r_v<FEdGraphNode*, LAMBDA>)
        void RegisterGraphNode(LAMBDA&& Lambda);

    
        
    protected:

        uint32 AddNode(FEdGraphNode* InNode);
        
        
        THashMap<FName, FNodeFactory>                   NodeFactories;
        TVector<FEdGraphNode*>                          Nodes;
        TVector<FAction>                                Actions;

    private:
        
        ImNodesEditorContext* ImNodesContext = nullptr;
    };



    
    
    template <typename T, typename... Args>
    requires (std::is_base_of_v<FEdGraphNode, T> && std::is_constructible_v<T, Args...>)
    T* FEdNodeGraph::CreateNode(Args&&... args)
    {
        T* New = FMemory::New<T>(TForward<Args>(args)...);

        AddNode(New);
        return New;
    }

    template <typename T, typename LAMBDA> requires (std::is_invocable_r_v<FEdGraphNode*, LAMBDA>)
    void FEdNodeGraph::RegisterGraphNode(LAMBDA&& Lambda)
    {
        FNodeFactory Factory;
        Factory.Name = T::Info::StaticDisplayName();
        Factory.Tooltip = T::Info::StaticTooltip();
        Factory.CreationCallback = TForward<LAMBDA>(Lambda);

        NodeFactories.insert_or_assign(Factory.Name.c_str(), Factory);
    }

    
}

#define REGISTER_GRAPH_NODE(NodeClass)                       \
       RegisterGraphNode<NodeClass>(                    \
           []() -> FEdGraphNode* { return NodeClass::Info::CreateInstance(); } \
       );                                                           \

