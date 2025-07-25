﻿#pragma once

#include "EdGraphNode.h"
#include "imnodes/imnodes_internal.h"
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Core/Templates/Forward.h"
#include "Memory/Memory.h"
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

        void SetNodeSelectedCallback(const TFunction<CEdGraphNode*()>& Callback) { NodeSelectedCallback = Callback; }
        
    protected:

        void RegisterGraphNode(CClass* InClass) { SupportedNodes.push_back(InClass); }
        
        uint64 AddNode(CEdGraphNode* InNode);

        TVector<CEdGraphNode*>                          Nodes;
        
        TVector<CClass*>                                SupportedNodes;
        TVector<FAction>                                Actions;

        TFunction<CEdGraphNode*()>                      NodeSelectedCallback;
    private:
        
        ImNodesEditorContext* ImNodesContext = nullptr;
    };
    
    
}
