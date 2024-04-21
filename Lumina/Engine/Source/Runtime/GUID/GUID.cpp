#include "GUID.h"

namespace Lumina
{

    static std::random_device RandomDevice;
    static std::mt19937_64 eng(RandomDevice());
    static std::uniform_int_distribution<uint64> UniformDistribution;


    FGuid FGuid::Generate()
    {
        return FGuid(UniformDistribution(eng));
    }
}
