#pragma once

#include "MaterialGraphTypes.h"
#include "Core/Object/ObjectMacros.h"
#include "Platform/GenericPlatform.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"
#include "MaterialInput.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CMaterialInput : public CEdNodeGraphPin
    {
        GENERATED_BODY()
        
    public:
        
        uint32 GetMask() const { return Mask; }
        void DrawPin() override;

        void SetInputType(EMaterialInputType InType) { InputType = InType; }

        void SetIndex(uint32 InIndex) { Index = InIndex; }
        uint32 GetIndex() const { return Index; }
        
    private:

        /** Mask represents the values of a float4 */
        uint32              Mask = 0;

        /** Index on the node's pin array, useful for offsetting for properties */
        uint32              Index = 0;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;

    };

}
