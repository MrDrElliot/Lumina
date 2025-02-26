#pragma once
#include "ViewVolume.h"
#include "Core/Math/Math.h"

namespace Lumina
{
    class FViewport
    {
    public:

        FViewport()
            :Size(FIntVector2D(0))
        {}

        FViewport(const FIntVector2D& InSize)
            :Size(InSize)
        {}
        
        FViewport(const FViewVolume& InVolume, const FIntVector2D& InSize)
            : ViewVolume(InVolume)
            , Size(InSize)
        {}

        FORCEINLINE const FViewVolume& GetViewVolume() const { return ViewVolume; }
        FORCEINLINE const FIntVector2D& GetSize() const { return Size; }

        FORCEINLINE void SetViewVolume(const FViewVolume& InVolume) { ViewVolume = InVolume;}
        FORCEINLINE void SetSize(const FIntVector2D& InSize) { Size = InSize; }

    private:

        FViewVolume        ViewVolume;
        FIntVector2D       Size; 
    
    };
}
