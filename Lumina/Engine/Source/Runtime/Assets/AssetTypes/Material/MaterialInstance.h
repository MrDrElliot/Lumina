#pragma once
#include "Core/Object/Object.h"
#include "Core/Object/ObjectMacros.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "MaterialInstance.generated.h"

namespace Lumina { class CMaterial; };

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CMaterialInstance : public CObject
    {
        GENERATED_BODY()
    public:
        
        

        LUM_PROPERTY()
        TObjectHandle<CMaterial> Material;
    };
}
