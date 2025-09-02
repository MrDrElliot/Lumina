#include "ObjectRedirector.h"

#include "Class.h"

namespace Lumina
{
    IMPLEMENT_INTRINSIC_CLASS(CObjectRedirector, CObject, LUMINA_API)

    void CObjectRedirector::Serialize(FArchive& Ar)
    {
        Ar << RedirectionObject;
    }

    
    void CObjectRedirector::Serialize(IStructuredArchive::FSlot Slot)
    {
        
    }

    
}
