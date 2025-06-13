#pragma once

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
        virtual ~CEdNodeGraph();

        virtual void Initialize();
        virtual void Shutdown();
        void Serialize(FArchive& Ar) override;

        void DrawGraph();
        virtual void OnDrawGraph();
        
        void RegisterGraphAction(const FString& ActionName, const TFunction<void()>& ActionCallback);

        virtual void ValidateGraph()  { };
        
        CEdGraphNode* CreateNode(CClass* NodeClass);
    
        
    protected:

        uint32 AddNode(CEdGraphNode* InNode);
        
        
        THashMap<FName, FNodeFactory>                   NodeFactories;
        TVector<CEdGraphNode*>                          Nodes;
        TVector<FAction>                                Actions;

    private:
        
        ImNodesEditorContext* ImNodesContext = nullptr;
    };
    
    
}
