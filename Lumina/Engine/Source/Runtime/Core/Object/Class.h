#pragma once

#include "Module/Api.h"
#include "Object.h"

namespace Lumina
{
    class FProperty;
}

namespace Lumina
{

    LUMINA_API void AllocateStaticClass(const TCHAR* Name, CClass** OutClass, uint32 Size, uint32 Alignment);

    /** Base class for any data structure that holds fields */
    class CStruct : public CObject
    {

        DECLARE_CLASS(CStruct, CObject, LUMINA_API)

        CStruct(FName InName, uint32 InSize, uint32 InAlignment, EObjectFlags InFlags)
            : CObject(nullptr, InFlags, InName)
            , Size(InSize)
            , Alignment(InAlignment)
        {
        }
        
        FORCEINLINE uint32 GetSize() const
        {
            return Size;
        }

        FORCEINLINE uint32 GetAlignment() const
        {
            return Alignment;
        }


        void AddField(FProperty* Property);
        FProperty* GetProperty(const FString& Name);
        
    private:
        
        FProperty* LinkedProperty;
        
        uint32 Size = 0;
        uint32 Alignment = 0;
    };

    /** Final class for fields and functions. */
    class CClass final : public CStruct
    {
    public:

        DECLARE_CLASS(CClass, CStruct, LUMINA_API)

        CClass(FName InName, uint32 InSize, uint32 InAlignment, EObjectFlags InFlags)
            : CStruct(InName, InSize, InAlignment, InFlags)
        {
        }


    private:
        
    };

}
