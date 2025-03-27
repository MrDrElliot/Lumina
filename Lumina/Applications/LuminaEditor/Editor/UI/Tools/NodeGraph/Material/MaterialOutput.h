#pragma once

#include "MaterialGraphTypes.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{
    class CMaterialOutput : public CEdNodeGraphPin
    {
    public:

        DECLARE_CLASS(CMaterialOutput, CEdNodeGraphPin)
        
        void DrawPin() override;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;
    };
}
