#pragma once

#include "UI/Tools/NodeGraph/EdNodeGraph.h"
#include "MaterialNodeGraph.generated.h"

namespace Lumina
{
    class CMaterial;
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

        void Initialize() override;
        
        void OnDrawGraph() override;
        void CompileGraph(FMaterialCompiler* Compiler);

        void ValidateGraph() override;

        void SetMaterial(CMaterial* InMaterial) { Material = InMaterial;}
        CMaterial* GetMaterial() const { return Material; }
        
    private:

        static CEdGraphNode* TopologicalSort(const TVector<CEdGraphNode*>& Nodes, TVector<CEdGraphNode*>& SortedNodes);

        CMaterial* Material = nullptr;
    };
}
