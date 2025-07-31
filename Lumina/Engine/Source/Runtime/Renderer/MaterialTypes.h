#pragma once
#include "Core/Object/ObjectMacros.h"
#include "glm/glm.hpp"
#include "MaterialTypes.generated.h"

#define MAX_VECTORS 24
#define MAX_SCALARS 24


namespace Lumina
{
    struct FMaterialUniforms
    {
        glm::vec4 Scalars[MAX_SCALARS / 4];
        glm::vec4 Vectors[MAX_VECTORS];
    };
    

    LUM_ENUM()
    enum class EMaterialParameterType : uint8
    {
        Scalar,
        Vector,
        Texture,
    };


    LUM_STRUCT()
    struct LUMINA_API FMaterialParameter
    {
        GENERATED_BODY()
        
        LUM_PROPERTY()
        FName ParameterName;

        LUM_PROPERTY()
        EMaterialParameterType Type;

        LUM_PROPERTY()
        uint16 Index;
    };
    
}
