#pragma once
#include "Core/Object/Class.h"
#include "Core/Object/ObjectCore.h"
#include "Core/Object/ObjectPtr.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FStructProperty : public FProperty
    {
    public:

        FStructProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
            auto* StructParams = (const FStructPropertyParams*)Params;
            CStruct* InternalStruct = StructParams->StructFunc();
            Assert(InternalStruct)
            SetStruct(InternalStruct);
            SetElementSize(Struct->GetSize(), Struct->GetAlignment());
        }

        void SetStruct(CStruct* InStruct) { Struct = InStruct; }
        CStruct* GetStruct() const { return Struct; }



        CStruct* Struct = nullptr;
    
    };
}
