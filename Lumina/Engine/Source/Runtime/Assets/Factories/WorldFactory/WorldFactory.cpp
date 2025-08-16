#include "WorldFactory.h"


namespace Lumina
{
    CObject* CWorldFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CWorld>(Package, Name);
    }
}
