#pragma once
#include "Archiver.h"
#include "ProxyArchive.h"

namespace Lumina
{
    class FObjectArchiver : public FProxyArchive
    {
    public:

        FObjectArchiver(FArchive& InInnerAr, bool bInLoadIfFindFails)
            : FProxyArchive(InInnerAr)
            , bLoadIfFindFails(bInLoadIfFindFails)
        {}
        
        
        LUMINA_API virtual FArchive& operator<<(CObject*& Obj) override;


    private:

        bool bLoadIfFindFails =false;
    };
}
