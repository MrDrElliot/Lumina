#include "MaterialInstanceFactory.h"


namespace Lumina
{
    CObject* CMaterialInstanceFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CMaterialInstance>(Package, Name);
    }
}
