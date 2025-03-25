#pragma once

#include "UI/Tools/NodeGraph/EdGraphNode.h"


namespace Lumina
{
    class FMaterialCompiler;
}


namespace Lumina
{
    class FMaterialGraphNode : public FEdGraphNode
    {
    public:

        FMaterialGraphNode()
            :bDynamic(false)
        {}
        
        virtual uint32 GenerateExpression(FMaterialCompiler* Compiler) = 0;
        virtual void GenerateDefinition(FMaterialCompiler* Compiler) = 0;
        
        FORCEINLINE bool IsDynamic() const { return bDynamic; }
        
        virtual float* GetNodeDefaultValue() { return nullptr; }

        
    protected:

        /** Is this node static or mutable? */
        uint8 bDynamic:1;
        
    };
    
}

