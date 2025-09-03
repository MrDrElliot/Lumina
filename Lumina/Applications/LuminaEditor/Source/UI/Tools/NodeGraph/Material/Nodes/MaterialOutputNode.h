#pragma once

#include "MaterialGraphNode.h"
#include "MaterialOutputNode.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CMaterialOutputNode : public CMaterialGraphNode
    {
        GENERATED_BODY()
    public:
        
        CMaterialOutputNode() = default;

        FString GetNodeDisplayName() const override;
        FString GetNodeTooltip() const override;
        uint32 GetNodeTitleColor() const override { return IM_COL32(35, 35, 200, 255); }

        void BuildNode() override;
        
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 1;}
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        bool IsDeletable() const override { return false; }

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
