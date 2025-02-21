#pragma once

#include "RHIFwd.h"
#include "Vertex.h"
#include "Viewport.h"
#include "Containers/Array.h"
#include "Scene/SceneRenderTypes.h"

namespace Lumina
{
    class FBatchedElements
    {
    public:

        FBatchedElements();
        ~FBatchedElements();

        void Initialize();
        
        void SubmitElement(const FSimpleElementVertex& Element);
        
        bool Draw(const FSceneGlobalData& GlobalData);

    private:

        TVector<FSimpleElementVertex>   LineVertices;
        FRHIBuffer                      VertexBuffer;
    };
}
