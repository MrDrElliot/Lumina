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
        void Shutdown() override;
        
        void OnDrawGraph() override;
        void CompileGraph(FMaterialCompiler* Compiler);

        void ValidateGraph() override;

        void SetMaterial(CMaterial* InMaterial);
        CMaterial* GetMaterial() const { return Material; }

        CEdGraphNode* CreateNode(CClass* NodeClass) override;

        
    private:

        static CEdGraphNode* TopologicalSort(const TVector<TObjectHandle<CEdGraphNode>>& Nodes, TVector<CEdGraphNode*>& SortedNodes);

        
        TObjectHandle<CMaterial> Material;
    };
}
