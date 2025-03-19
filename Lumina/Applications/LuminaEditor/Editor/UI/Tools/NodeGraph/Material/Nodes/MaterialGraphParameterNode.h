#pragma once
#include "MaterialGraphNode.h"

namespace Lumina
{
    class FMaterialGraphParameterNode : public FMaterialGraphNode
    {
    public:
    
        uint32 GetNodeTitleColor() const override { return IM_COL32(35, 165, 35, 255); }

    protected:
        
    };
}
