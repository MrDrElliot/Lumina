#pragma once
#include "Core/Object/ConstructObjectParams.h"
#include "Module/API.h"

namespace Lumina
{
    class CObject;
    class CPackage;
}

namespace Lumina
{
    
    class FObjectInitializer
    {
    public:

        LUMINA_API FObjectInitializer()
            : Object(nullptr)
            , Package(nullptr)
            , Params(nullptr)
        {}
        
        LUMINA_API FObjectInitializer(CObject* Obj, CPackage* InPackage, const FConstructCObjectParams& InParams);
        ~FObjectInitializer();

        static FObjectInitializer* Get();
        CObject* GetObj() const { return Object; }


        void Construct();

        CObject* Object;
        CPackage* Package;
        FConstructCObjectParams Params;
    };
    
}
