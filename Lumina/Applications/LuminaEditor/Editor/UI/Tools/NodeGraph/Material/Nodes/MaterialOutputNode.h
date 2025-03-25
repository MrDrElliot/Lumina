#pragma once

#include "MaterialGraphNode.h"

namespace Lumina
{
    class FMaterialOutputNode : public FMaterialGraphNode
    {
    public:

        FMaterialOutputNode() = default;

        FString GetNodeDisplayName() const override;
        FString GetNodeTooltip() const override;
        uint32 GetNodeTitleColor() const override { return IM_COL32(35, 35, 200, 255); }

        void BuildNode() override;
        
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 1;}
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        FEdNodeGraphPin* BaseColorPin = nullptr;
        FEdNodeGraphPin* MetallicPin = nullptr;
        FEdNodeGraphPin* RoughnessPin = nullptr;
        FEdNodeGraphPin* SpecularPin = nullptr;
        FEdNodeGraphPin* EmissivePin = nullptr;
        FEdNodeGraphPin* AOPin = nullptr;
        FEdNodeGraphPin* NormalPin = nullptr;
        FEdNodeGraphPin* OpacityPin = nullptr;
    
    };
}
