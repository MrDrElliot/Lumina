#include "ObjectHandle.h"

#include "ObjectArray.h"

namespace Lumina
{
    FObjectHandle::FObjectHandle(nullptr_t)
    {
    }

    FObjectHandle::FObjectHandle(uint32 InIndex, uint32 InGeneration)
        : Index(InIndex)
        , Generation(InGeneration)
    {
    }

    FObjectHandle::FObjectHandle(const CObject* Object)
    {
        *this = GObjectArray.ToHandle((const CObjectBase*)Object);
    }

    CObject* FObjectHandle::Resolve() const
    {
        return (CObject*)GObjectArray.Resolve(*this);
    }
}
