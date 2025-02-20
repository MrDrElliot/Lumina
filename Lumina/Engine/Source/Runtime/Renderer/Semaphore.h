#pragma once
#include "RenderResource.h"


namespace Lumina
{
    class FSemaphore : public IRenderResource
    {

    public:

        FSemaphore() = default;
        virtual ~FSemaphore() = default;
        
        static TRefCountPtr<FSemaphore> Create();


    private:
        
    };
}
