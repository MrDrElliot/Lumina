#pragma once
#include "Core/LuminaMacros.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{

    enum class EMaterialValueType : uint8
    {
        Float,
        Float2,
        Float3,
        Float4,
    };
    
    class FMaterialNodePin : public FEdNodeGraphPin
    {
    public:
        
        void DrawPin() override;

    private:
        
    };
}
