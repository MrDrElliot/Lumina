#pragma once
#include "MaterialGraphTypes.h"
#include "Platform/GenericPlatform.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{
    class FMaterialInput : public FEdNodeGraphPin
    {
    public:

        FMaterialInput(EMaterialInputType Type)
            :InputType(Type)
        {}
        

        uint32 GetMask() const { return Mask; }
        
        void DrawPin() override;
        
    private:

        /** Mask represents the values of a float4 */
        uint32              Mask = 0;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;

    };
}
