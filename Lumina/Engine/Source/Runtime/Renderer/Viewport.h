#pragma once
#include "ViewVolume.h"
#include "Core/Math/Math.h"

namespace Lumina
{
    class FViewport
    {
    public:

        FViewport(const FViewVolume& InVolume, const FVector2D& InSize)
            : ViewVolume(InVolume)
            , Size(InSize)
        {}

        FORCEINLINE const FViewVolume& GetViewVolume() const { return ViewVolume; }
        FORCEINLINE const FVector2D& GetSize() const { return Size; }


    private:

        FViewVolume     ViewVolume;
        FVector2D       Size; 
    
    };
}
