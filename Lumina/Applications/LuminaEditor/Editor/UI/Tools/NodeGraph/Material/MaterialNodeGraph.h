#pragma once
#include <sstream>
#include <string>

#include "Core/LuminaMacros.h"
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

    private:

        static FEdGraphNode* TopologicalSort(const TVector<FEdGraphNode*>& Nodes, TVector<FEdGraphNode*>& SortedNodes);
    
    };
}
