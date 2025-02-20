#pragma once

#include "Vertex.h"
#include "Viewport.h"
#include "Containers/Array.h"

namespace Lumina
{
    class FBatchedElements
    {
    public:
        
        bool Draw(const FViewport& ViewVolume);

    private:

        TVector<FSimpleElementVertex> LineVertices;
    };
}
