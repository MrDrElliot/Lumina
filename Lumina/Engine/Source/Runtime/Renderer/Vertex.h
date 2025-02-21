#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Math.h"
#include "Core/Templates/CanBulkSerialize.h"



namespace Lumina
{
    struct FVertex
    {
        FVector3D Position;
        FColor    Color;
        FVector3D Normal;
        FVector2D UV;
        
        friend FArchive& operator << (FArchive& Ar, FVertex& data)
        {
            Ar << data.Position;
            Ar << data.Color;
            Ar << data.Normal;
            Ar << data.UV;
            return Ar;
        }
    };

    struct FSimpleElementVertex
    {
        FVector3D   Position;
        FColor      Color;
    };
    
    
    template<>
    struct TCanBulkSerialize<FVertex> { enum { Value = true }; };

    template<>
    struct TCanBulkSerialize<FSimpleElementVertex> { enum { Value = true }; };
    
}
