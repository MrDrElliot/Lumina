#pragma once

#include "UI/Tools/NodeGraph/EdNodeGraph.h"

namespace Lumina
{
    class FMaterialCompiler;
}

namespace Lumina
{
    class FMaterialNodeGraph : public FEdNodeGraph
    {
    public:

        FMaterialNodeGraph();

        void OnDrawGraph() override;
        void CompileGraph(FMaterialCompiler* Compiler);

        void ValidateGraph() override;

    private:

        static FEdGraphNode* TopologicalSort(const TVector<FEdGraphNode*>& Nodes, TVector<FEdGraphNode*>& SortedNodes);
    
    };
}
