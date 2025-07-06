#pragma once

#include "Lumina.h"
#include "UI/Tools/NodeGraph/EdGraphNode.h"
#include "MaterialGraphNode.generated.h"

namespace Lumina
{
    class FMaterialCompiler;
}

namespace Lumina
{
    
    LUM_CLASS()
    class CMaterialGraphNode : public CEdGraphNode
    {
        GENERATED_BODY()
        
    public:
        
        CMaterialGraphNode()
            : bDynamic(false)
        { }
        
        virtual uint32 GenerateExpression(FMaterialCompiler* Compiler) { return INDEX_NONE; }
        virtual void GenerateDefinition(FMaterialCompiler* Compiler) { };
        
        bool IsDynamic() const { return bDynamic; }
        
        virtual void* GetNodeDefaultValue() { return nullptr; }
        virtual void SetNodeValue(void* Value) { }

        
    protected:

        /** Is this node static or mutable? */
        uint8 bDynamic:1;
        
    };
    
}

