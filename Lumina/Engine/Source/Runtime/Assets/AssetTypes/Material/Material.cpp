#include "Material.h"

namespace Lumina
{
    void AMaterial::Serialize(FArchive& Ar)
    {
        Ar << GraphData;
    }
}