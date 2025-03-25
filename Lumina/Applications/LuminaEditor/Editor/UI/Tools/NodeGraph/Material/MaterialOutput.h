#pragma once

#include "MaterialGraphTypes.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{
    class FMaterialOutput : public FEdNodeGraphPin
    {
    public:

        FMaterialOutput(EMaterialInputType Type)
            :InputType(Type)
        {}

        void DrawPin() override;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;
    };
}
