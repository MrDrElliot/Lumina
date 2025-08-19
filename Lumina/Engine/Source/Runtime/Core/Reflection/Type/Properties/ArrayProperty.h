#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Lumina.h"

namespace Lumina
{

    
    class FArrayProperty : public FProperty
    {
    public:

        
        FArrayProperty(const FFieldOwner& InOwner, const FArrayPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
            PushBackFn = Params->PushBackFn;
            GetNumFn = Params->GetNumFn;
            RemoveAtFn = Params->RemoveAtFn;
            ClearFn = Params->ClearFn;
            GetAtFn = Params->GetAtFn;
        }
        
        ~FArrayProperty() override = default;

        void AddProperty(FProperty* Property) override { Inner = Property; }

        void Serialize(FArchive& Ar, void* Value) override;
        void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;

        FProperty* GetInternalProperty() const { return Inner; }
        
        SIZE_T GetNum(const void* InContainer) const
        {
            return GetNumFn(InContainer);
        }

        void PushBack(void* InContainer, const void* InValue) const
        {
            PushBackFn(InContainer, InValue);
        }

        void RemoveAt(void* InContainer, size_t Index) const
        {
            RemoveAtFn(InContainer, Index);
        }

        void Clear(void* InContainer) const
        {
            ClearFn(InContainer);
        }

        void* GetAt(void* InContainer, size_t Index) const
        {
            return GetAtFn(InContainer, Index);
        }

        template<typename T = void, typename TFunc>
        void ForEach(void* InContainer, TFunc&& Func) const
        {
            SIZE_T Num = GetNum(InContainer);
            for (SIZE_T i = 0; i < Num; ++i)
            {
                if constexpr (std::is_same_v<T, void>)
                {
                    void* Elem = GetAt(InContainer, i);
                    Func(Elem, i);
                }
                else
                {
                    T* Elem = static_cast<T*>(GetAt(InContainer, i));
                    Func(Elem, i);
                }
            }
        }
        
    private:

        ArrayPushBackPtr    PushBackFn;
        ArrayGetNumPtr      GetNumFn;
        ArrayRemoveAtPtr    RemoveAtFn;
        ArrayClearPtr       ClearFn;
        ArrayGetAtPtr       GetAtFn;
        FProperty* Inner = nullptr;
        
    };

    /**
     * Minimal type-erased vector view.
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
            ElementSize = (uint16)InArrayProperty->GetInternalProperty()->GetElementSize();
        }

        
        
        NODISCARD void* GetData() const
        {
            return begin();
        }

        NODISCARD void* GetVectorPointer() const
        {
            return VectorPtr;
        }
        
        NODISCARD bool IsValidIndex(int32 i) const
        {
            return i >= 0 && i < Num();
        }

        NODISCARD bool IsEmpty() const
        {
            return Num() == 0;
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

            return (begin() + Index * Property->GetInternalProperty()->ElementSize);
        }

        void Remove(SIZE_T Index)
        {
            const SIZE_T Count = Num();
            if (Index >= Count)
                return;

            uint8* Base = begin();
            uint8* RemovePtr = Base + Index * ElementSize;

            Memory::Memzero(RemovePtr, ElementSize);

            if (Index < Count - 1)
            {
                void* Src = RemovePtr + ElementSize;
                void* Dst = RemovePtr;
                SIZE_T MoveSize = (Count - Index - 1) * ElementSize;
                Memory::Memcpy(Dst, Src, MoveSize);
            }

            Resize(Count - 1);
        }

        void Resize(SIZE_T NewElementCount)
        {

            uint8* OldBegin = begin();
            uint8* OldEnd = end();
            uint8* OldCapacity = capacity();
            SIZE_T OldElementCount = (OldEnd - OldBegin) / ElementSize;
            SIZE_T CapacityBytes = OldCapacity - OldBegin;
            SIZE_T CapacityElements = CapacityBytes / ElementSize;

            if (NewElementCount <= CapacityElements)
            {
                uint8* NewEnd = OldBegin + NewElementCount * ElementSize;
        
                if (NewElementCount > OldElementCount)
                {
                    // Zero new memory
                    SIZE_T BytesToZero = (NewElementCount - OldElementCount) * ElementSize;
                    Memory::Memzero(OldEnd, BytesToZero);
                }

                *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) = NewEnd;
            }
            else
            {
                // Need to reallocate
                SIZE_T NewCapacity = eastl::max(CapacityElements * 2, NewElementCount);
                SIZE_T AllocSize = NewCapacity * ElementSize;

                uint8* NewData = (uint8*)Memory::Malloc(AllocSize);
                Memory::Memzero(NewData, AllocSize);

                if (OldBegin)
                {
                    Memory::Memcpy(NewData, OldBegin, OldElementCount * ElementSize);
                    Memory::Free(OldBegin);
                }

                uint8* NewEnd = NewData + NewElementCount * ElementSize;
                uint8* NewCapacityPtr = NewData + AllocSize;

                *reinterpret_cast<uint8**>((uint8*)VectorPtr + 0) = NewData;
                *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*) * 1) = NewEnd;
                *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*) * 2) = NewCapacityPtr;
            }
        }

        void Add(int32 Count)
        {
            if (Count <= 0)
                return;

            const FProperty* Inner = Property->GetInternalProperty();
            const SIZE_T ElementSize = Inner->ElementSize;

            SIZE_T OldCount = Num();
            SIZE_T NewCount = OldCount + Count;

            uint8* Begin = begin();
            uint8* End = end();
            uint8* CapacityEnd = capacity();
            SIZE_T CurrentCapacity = (CapacityEnd - Begin) / ElementSize;

            if (NewCount > CurrentCapacity)
            {
                SIZE_T NewCapacity = eastl::max(NewCount, CurrentCapacity * 2);
                SIZE_T NewBytes = NewCapacity * ElementSize;

                uint8* NewData = (uint8*)Memory::Malloc(NewBytes);
                Memory::Memzero(NewData, NewBytes);

                SIZE_T OldBytes = OldCount * ElementSize;
                if (Begin)
                {
                    Memory::Memcpy(NewData, Begin, OldBytes);
                    Memory::Free(Begin);
                }

                Begin = NewData;
                End = Begin + OldCount * ElementSize;
                CapacityEnd = Begin + NewBytes;

                *reinterpret_cast<uint8**>(VectorPtr) = Begin;
                *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) = End;
                *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*) * 2) = CapacityEnd;
            }
            
            // Update end pointer
            *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) += Count * ElementSize;
        }

        void Clear()
        {
            uint8* Begin = begin();
            
            SIZE_T CapacityBytes = capacity() - Begin;
            Memory::Memzero(Begin, CapacityBytes);

            // Set end pointer = begin
            *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) = Begin;
        }

        void Reset()
        {
            uint8* Begin = begin();

            // Free memory
            eastl::destruct(begin(), end());
            Memory::Free(Begin);

            // Null the vector pointers
            *reinterpret_cast<uint8**>(VectorPtr) = nullptr;
            *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*)) = nullptr;
        }

        uint8* capacity() const
        {
            return *reinterpret_cast<uint8**>((uint8*)VectorPtr + sizeof(void*) * 2);
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
        uint16                  ElementSize;
        const FArrayProperty*   Property;
    };
}
