#pragma once
#include "Archiver.h"

namespace Lumina
{
    class FProxyArchive : public FArchive
    {
    public:

        FProxyArchive(FArchive& InInnerAr)
            :InnerArchive(InInnerAr)
        {}
    
    protected:
        
        FArchive& InnerArchive;
        
    };
}
