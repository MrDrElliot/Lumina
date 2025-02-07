#pragma once
#include "RenderResource.h"


namespace Lumina
{
    class FSemaphore : public FRenderResource
    {

    public:

        FSemaphore() = default;
        virtual ~FSemaphore() = default;
        
        static TRefPtr<FSemaphore> Create();


    private:
        
    };
}
