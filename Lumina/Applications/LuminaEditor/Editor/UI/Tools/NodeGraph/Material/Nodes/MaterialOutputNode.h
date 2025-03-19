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

        FString Evaluate(FMaterialCompiler* Compiler) override;
        
    
    };
}
