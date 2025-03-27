#pragma once

#include "MaterialGraphNode.h"

namespace Lumina
{
    class CMaterialOutputNode : public CMaterialGraphNode
    {
    public:

        DECLARE_CLASS(CMaterialOutputNode, CMaterialGraphNode)
        
        CMaterialOutputNode() = default;

        FString GetNodeDisplayName() const override;
        FString GetNodeTooltip() const override;
        uint32 GetNodeTitleColor() const override { return IM_COL32(35, 35, 200, 255); }

        void BuildNode() override;
        
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 1;}
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        CEdNodeGraphPin* BaseColorPin = nullptr;
        CEdNodeGraphPin* MetallicPin = nullptr;
        CEdNodeGraphPin* RoughnessPin = nullptr;
        CEdNodeGraphPin* SpecularPin = nullptr;
        CEdNodeGraphPin* EmissivePin = nullptr;
        CEdNodeGraphPin* AOPin = nullptr;
        CEdNodeGraphPin* NormalPin = nullptr;
        CEdNodeGraphPin* OpacityPin = nullptr;
    
    };
    
}
