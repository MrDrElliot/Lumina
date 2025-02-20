#pragma once

#include <glm/glm.hpp>
#include "Memory/RefCounted.h"
#include "Image.h"
#include "Assets/Asset.h"
#include "Platform/GenericPlatform.h"
#include "DescriptorSet.h"
#include <Containers/Name.h>


namespace Lumina
{
    class FBuffer;
    class FShader;
    class FPipeline;
    
    class AMaterial : public IAsset
    {
    public:

        DECLARE_ASSET("Material", Material, 1)
        
        AMaterial();
        ~AMaterial() override;
        
        /** Binds this material to the renderer */
        void Bind();

    protected:

        TRefCountPtr<FPipeline> QueryPipeline();
        TRefCountPtr<FShader> QueryShader() const;

    private:

        FRHIPipeline                                Pipeline;
        
    };
    
}
