#include "ArchetypeFactory.h"

namespace Lumina
{
    CObject* CArchetypeFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CArchetype>(Package, Name);
    }
}