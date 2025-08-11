#pragma once

namespace Lumina
{
    class IRHIResource;
}

namespace Lumina
{
    class FRGResource
    {
    public:

        virtual ~FRGResource() = default;

    private:
        
        IRHIResource* RHIResource = nullptr;
    };


    class FRGBuffer : public FRGResource
    {
    public:

        ~FRGBuffer() override { }
        
    };

    class FRGImage : public FRGResource
    {
    public:

        ~FRGImage() override { }
        
    };
}
