#pragma once

#include "MaterialGraphTypes.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{
    class CMaterialOutput : public CEdNodeGraphPin
    {
    public:
        
        void DrawPin() override;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;
    };
}
