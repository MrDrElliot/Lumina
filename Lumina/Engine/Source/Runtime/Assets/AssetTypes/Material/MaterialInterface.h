#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Core/Object/Object.h"
#include "MaterialInterface.generated.h"

namespace Lumina
{
    class CMaterial;
    struct FMaterialParameter;
    enum class EMaterialParameterType : uint8;
}

namespace Lumina
{
    
    LUM_CLASS()
    class LUMINA_API CMaterialInterface : public CObject
    {
        GENERATED_BODY()
    public:

        virtual CMaterial* GetMaterial() const { LUMINA_NO_ENTRY(); }
        virtual bool SetVectorValue(const FName& Name, const glm::vec4& Value) { LUMINA_NO_ENTRY(); }
        virtual bool SetScalarValue(const FName& Name, const float Value) { LUMINA_NO_ENTRY(); }
        virtual bool GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param) { LUMINA_NO_ENTRY(); }

        // @TODO TEMP
        virtual bool IsReadyForRender() const { return false; }
        
    };
}
