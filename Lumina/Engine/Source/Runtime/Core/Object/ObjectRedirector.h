#pragma once
#include "Object.h"

namespace Lumina
{
    /** Redirects a previous reference to an object to a new reference. */
    class CObjectRedirector : public CObject
    {
    public:

        DECLARE_CLASS(Lumina, CObjectRedirector, CObject, "script://lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CObjectRedirector)


        void Serialize(FArchive& Ar) override;
        void Serialize(IStructuredArchive::FSlot Slot) override;
        
        CObject* RedirectionObject;
    
    };
}
