#pragma once
#include "Core/Object/ObjectCore.h"
#include "Module/API.h"

namespace Lumina
{
    class CObject;
}

namespace Lumina
{
    
    class FObjectInitializer
    {
    public:

        LUMINA_API FObjectInitializer()
            : Object(nullptr)
            , Params(nullptr)
        {}
        
        LUMINA_API FObjectInitializer(CObject* Obj, const FConstructCObjectParams& InParams);
        ~FObjectInitializer();

        static FObjectInitializer* Get();
        CObject* GetObj() const { return Object; }


        void Construct();

        CObject* Object;
        FConstructCObjectParams Params;
        
    };
    
}
