﻿#pragma once
#include "Archiver.h"
#include "ProxyArchive.h"

namespace Lumina
{

    class FObjectArchive : public FArchive
    {
    public:
        
        virtual FArchive& operator<<(CObject*& Value) override
        {
            return *this;
        }
    };
    
    class FObjectProxyArchiver : public FProxyArchive
    {
    public:

        FObjectProxyArchiver(FArchive& InInnerAr, bool bInLoadIfFindFails)
            : FProxyArchive(InInnerAr)
            , bLoadIfFindFails(bInLoadIfFindFails)
        {}
        
        
        LUMINA_API virtual FArchive& operator<<(CObject*& Obj) override;


    private:

        bool bLoadIfFindFails =false;
    };
}
