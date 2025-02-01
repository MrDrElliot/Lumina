#include "MaterialInstance.h"


namespace Lumina
{
    void AMaterialInstance::Serialize(FArchive& Ar)
    {
        DEFINE_ASSET_HEADER()
            ADD_DEPENDENCY(Albedo)
            ADD_DEPENDENCY(Normal)
            ADD_DEPENDENCY(Roughness)
            ADD_DEPENDENCY(Emissive)
            ADD_DEPENDENCY(AmbientOcclusion)
        SERIALIZE_HEADER(Ar)

    }
}
