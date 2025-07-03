#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Lumina.h"

namespace Lumina
{

    
    class FArrayProperty : public FProperty
    {
    public:

        
        FArrayProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
        }
        
        ~FArrayProperty() override = default;

        void AddProperty(FProperty* Property) override { Inner = Property; }

        void Serialize(FArchive& Ar, void* Value) override;
        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;

        FProperty* GetInternalProperty() const { return Inner; }

        void DrawProperty(void* Object) override;
    
    private:

        FProperty* Inner = nullptr;
    
    };

    /**
     * Mininal type-erased vector view.
     */
    class FReflectArrayHelper
    {
    public:
        
        /**
         * 
         * @param InArrayProperty Property associated.
         * @param ContainerPtr Pointer to the raw vector memory.
         */
        FReflectArrayHelper(const FArrayProperty* InArrayProperty, void* ContainerPtr)
            : Property(InArrayProperty)
        {
            VectorPtr = ContainerPtr;
        }

        void* GetData() const
        {
            return VectorPtr;
        }

        void* GetVectorPointer() const
        {
            return VectorPtr;
        }

        SIZE_T Num() const
        {
            const uint8* b = begin();
            const uint8* e = end();
            return (e - b) / Property->GetInternalProperty()->ElementSize;
        }

        void* GetRawAt(SIZE_T Index) const
        {
            SIZE_T count = Num();
            if (Index >= count)
            {
                return nullptr;
            }

            return (void*)(begin() + Index * Property->GetInternalProperty()->ElementSize);
        }

        void Resize(SIZE_T NewElementCount)
        {
            if (NewElementCount <= 0)
            {
                return;
            }
            
            uint8* OldData = begin();
            if (OldData)
            {
                Memory::Free(OldData);
            }

            SIZE_T ElementSize = Property->GetInternalProperty()->ElementSize;
            SIZE_T TotalBytes = NewElementCount * ElementSize;

            uint8* NewData = (uint8*)Memory::Malloc(TotalBytes);
            Memory::Memzero(NewData, TotalBytes);

            *reinterpret_cast<uint8**>(VectorPtr) = NewData;
            *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) = NewData + TotalBytes;
        }
        
        uint8* begin() const
        {
            return *reinterpret_cast<uint8**>(VectorPtr);
        }

        uint8* end() const
        {
           return *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(VectorPtr) + sizeof(void*));
        }


    private:
        
        void*                   VectorPtr;
        const FArrayProperty*   Property;
    };
}
