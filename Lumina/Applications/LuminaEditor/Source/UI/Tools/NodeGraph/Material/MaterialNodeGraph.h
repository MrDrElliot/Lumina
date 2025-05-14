#pragma once

#include "UI/Tools/NodeGraph/EdNodeGraph.h"
#include "MaterialNodeGraph.generated.h"

namespace Lumina
{
    class FMaterialCompiler;
}

namespace Lumina
{
    LUM_CLASS()
    class CMaterialNodeGraph : public CEdNodeGraph
    {
        GENERATED_BODY()
        
    public:
        
        CMaterialNodeGraph();

        void OnDrawGraph() override;
        void CompileGraph(FMaterialCompiler* Compiler);

        void ValidateGraph() override;

    private:

        static CEdGraphNode* TopologicalSort(const TVector<CEdGraphNode*>& Nodes, TVector<CEdGraphNode*>& SortedNodes);
    
    };
}
