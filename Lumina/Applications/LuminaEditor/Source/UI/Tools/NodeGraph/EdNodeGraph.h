#pragma once

#include "EdGraphNode.h"
#include "imnodes/imnodes_internal.h"
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "EdNodeGraph.generated.h"

namespace Lumina
{
    class CEdGraphNode;
}

namespace Lumina
{
    LUM_CLASS()
    class CEdNodeGraph : public CObject
    {
        GENERATED_BODY()
        
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
            TFunction<CEdGraphNode*()> CreationCallback;
        };

        
        CEdNodeGraph();
        virtual ~CEdNodeGraph() override;

        virtual void Initialize();
        virtual void Shutdown();
        void Serialize(FArchive& Ar) override;
        
        void DrawGraph();
        virtual void OnDrawGraph();
        
        void RegisterGraphAction(const FString& ActionName, const TFunction<void()>& ActionCallback);

        virtual void ValidateGraph()  { }
        
        virtual CEdGraphNode* CreateNode(CClass* NodeClass);

        virtual CEdGraphNode* OnNodeRemoved(CEdGraphNode* Node) { return nullptr; }

        void SetNodeSelectedCallback(const TFunction<void(CEdGraphNode*)>& Callback) { NodeSelectedCallback = Callback; }
        
    public:

        void RegisterGraphNode(CClass* InClass);
        
        uint64 AddNode(CEdGraphNode* InNode);

        LUM_PROPERTY()
        TVector<TObjectHandle<CEdGraphNode>>            Nodes;
        
        LUM_PROPERTY()
        TVector<uint16>                                 Connections;
        
        THashSet<CClass*>                               SupportedNodes;
        TVector<FAction>                                Actions;
        TQueue<CEdGraphNode*>                           NodesToDestroy;
        
        TFunction<void(CEdGraphNode*)>                  NodeSelectedCallback;
    private:
        
        ImNodesEditorContext* ImNodesContext = nullptr;
    };
    
    
}
