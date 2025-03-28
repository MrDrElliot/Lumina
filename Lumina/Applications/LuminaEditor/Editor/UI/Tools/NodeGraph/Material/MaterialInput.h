#pragma once

#include "MaterialGraphTypes.h"
#include "Core/Object/Field.h"
#include "Core/Object/ObjectMacros.h"
#include "Platform/GenericPlatform.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"

namespace Lumina
{
    class CMaterialInput : public CEdNodeGraphPin
    {
    public:

        DECLARE_CLASS(CMaterialInput, CEdNodeGraphPin)
        static FClassMemberData RegisterClassMembers()
        {
            
        }
        /*BEGIN_CLASS_DATA()
            DEFINE_FIELD(Mask, Integer)
            DEFINE_FIELD(InputType, Enum)
        END_CLASS_DATA()*/

        uint32 GetMask() const { return Mask; }
        
        void DrawPin() override;
        
    private:

        /** Mask represents the values of a float4 */
        uint32              Mask = 0;
        
        EMaterialInputType  InputType = EMaterialInputType::Float;

    };

}
