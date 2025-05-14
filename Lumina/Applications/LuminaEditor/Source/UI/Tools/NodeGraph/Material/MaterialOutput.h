#pragma once

#include "MaterialGraphTypes.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"
#include "MaterialOutput.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CMaterialOutput : public CEdNodeGraphPin
    {
        GENERATED_BODY()
    public:
        
        void DrawPin() override;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;
    };
}
