#pragma once

#include "UI/Tools/NodeGraph/EdNodeGraph.h"

namespace Lumina
{
    class FMaterialCompiler;
}

namespace Lumina
{
    class CMaterialNodeGraph : public CEdNodeGraph
    {
    public:
        
        CMaterialNodeGraph();

        void OnDrawGraph() override;
        void CompileGraph(FMaterialCompiler* Compiler);

        void ValidateGraph() override;

    private:

        static CEdGraphNode* TopologicalSort(const TVector<CEdGraphNode*>& Nodes, TVector<CEdGraphNode*>& SortedNodes);
    
    };
}
