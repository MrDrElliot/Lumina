#pragma once
#include <random>

#include "Core/Serialization/Archiver.h"


namespace Lumina
{

    class FGuid
    {
    public:

        FGuid():Guid(0) {}

        static FGuid Generate();
        uint64_t Get() const { return Guid; }
        bool IsValid() const { return Guid != 0; }

        operator uint64() const { return Guid; }

        friend FArchive& operator << (FArchive& Ar, FGuid& Data)
        {
            Ar << Data.Guid;

            return Ar;
        }
    
    private:

        FGuid(uint64 New):Guid(New) {}
        uint64 Guid;
    };
}
