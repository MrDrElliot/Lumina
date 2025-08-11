#pragma once
#include "Core/Templates/Optional.h"


namespace Lumina
{
    class FRGImage;
    class FRGBuffer;
    class FRGResource;
}


namespace Lumina
{

    class FRGPassDescriptor
    {
    public:
        
        struct FAttachment
        {
            FRGImage*       Image;
            ERenderLoadOp   LoadOp;
            ERenderStoreOp  StoreOp;
        };

        FRGPassDescriptor& AddSRVBuffer(FRGBuffer* Buffer)
        {
            SRVBuffers.push_back(Buffer);
            return *this;
        }

        FRGPassDescriptor& AddSRVImage(FRGImage* Image)
        {
            SRVImages.push_back(Image);
            return *this;
        }

        FRGPassDescriptor& AddUAVBuffer(FRGBuffer* Buffer)
        {
            UAVBuffers.push_back(Buffer);
            return *this;
        }

        FRGPassDescriptor& AddUAVImage(FRGImage* Image)
        {
            UAVImages.push_back(Image);
            return *this;
        }

        FRGPassDescriptor& AddColorAttachment(FRGImage* Image, ERenderLoadOp LoadOp, ERenderStoreOp StoreOp)
        {
            ColorAttachments.push_back({ Image, LoadOp, StoreOp });
            return *this;
        }

        FRGPassDescriptor& SetDepthAttachment(FRGImage* Image, ERenderLoadOp LoadOp, ERenderStoreOp StoreOp)
        {
            DepthAttachment = FAttachment{ Image, LoadOp, StoreOp };
            return *this;
        }

    private:
        TVector<FRGBuffer*> UAVBuffers;
        TVector<FRGImage*>  UAVImages;
        TVector<FRGBuffer*> SRVBuffers;
        TVector<FRGImage*>  SRVImages;

        TVector<FAttachment> ColorAttachments;
        TOptional<FAttachment> DepthAttachment;
    };
}
