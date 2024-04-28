#pragma once
#include <random>


namespace Lumina
{
    typedef uint64_t uint64;

    class FGuid
    {
    public:
        FGuid():Guid(0) {}

        static FGuid Generate();
        uint64 Get() const { return Guid; }
        bool IsValid() const { return Guid != 0; }

        operator uint64() const { return Guid; }
    
    private:

        FGuid(uint64 New):Guid(New) {}
        uint64 Guid;
    };
}
