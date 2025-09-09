#pragma once

#include "EdGraphNode.h"
#include "Containers/Array.h"
#include "Containers/Function.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "EdNodeGraph.generated.h"
#include "imgui-node-editor/imgui_node_editor.h"

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
        ~CEdNodeGraph() override;

        virtual void Initialize();
        virtual void Shutdown();
        void Serialize(FArchive& Ar) override;
        
        void DrawGraph();
        virtual void DrawGraphContextMenu();
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

        THashMap<uint64, SIZE_T> NodeIDToIndex;

    private:
        
        ax::NodeEditor::EditorContext* Context = nullptr;
    };
    
    
}
