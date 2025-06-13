#pragma once
#include "Core/Object/Field.h"
#include "Core/Serialization/Structured/StructuredArchive.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FPropertyParams;
}

namespace Lumina
{
    class IStructuredArchive;
}

namespace Lumina
{
    class FProperty : public FField
    {
    public:

        FProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FField(InOwner)
        {
            Init();
        }
        
        // Adds self to owner.
        void Init();


        template<typename ValueType>
        ValueType* SetValuePtr(void* ContainerPtr, const ValueType& Value, int32 ArrayIndex = 0)
        {
            if (sizeof(Value) != Size)
            {
                return nullptr;
            }
            
            ValueType* ValuePtr = GetValuePtr<ValueType>(ContainerPtr, ArrayIndex);
            *ValuePtr = Value;
            return ValuePtr;
        }

        /** Gets the casted internal value type by an offset, UB if type is not correct */
        template<typename ValueType>
        ValueType* GetValuePtr(void* ContainerPtr, int32 ArrayIndex = 0) const
        {
            return (ValueType*)GetValuePtrInternal(ContainerPtr, ArrayIndex);
        }

        virtual void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) { }

    private:

        LUMINA_API void* GetValuePtrInternal(void* ContainerPtr, int32 ArrayIndex) const;
        
    public:
        
        uint32 Size;
        uint32 ElementSize;

        /** Linked list of properties from most-derived to base */
        FProperty* PropertyLinkNext;
        
    };

    class FNumericProperty : public FProperty
    {
    public:

        FNumericProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {}
        
    void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults) override;

    };
    

    template<typename TCPPType>
    class TPropertyTypeLayout
    {
    public:

        enum
        {
            Size = sizeof(TCPPType),
            Alignment = alignof(TCPPType)
        };

        /** Convert the address of a value of the property to the proper type */
        static FORCEINLINE const TCPPType* GetPropertyValuePtr(const void* Ptr)
        {
            return (const TCPPType*)Ptr;
        }

        /** Convert the address of a value of the property to the proper type */
        static FORCEINLINE TCPPType* GetPropertyValuePtr(void* Ptr)
        {
            return (TCPPType*)Ptr;
        }

        /** Set the value of a property at an address */
        static FORCEINLINE void SetPropertyValue(void* Ptr, const TCPPType& Value)
        {
            *GetPropertyValuePtr(Ptr) = Value;
        }
        
    };
    
    template<typename TBacking, typename TCPPType>
    class TProperty : public TBacking, public TPropertyTypeLayout<TCPPType>
    {
    public:

        TProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :TBacking(InOwner, Params)
        {
            this->ElementSize = TPropertyTypeLayout<TCPPType>::Size;
        }
        
        void SetValue(void* OwnerPtr, const TCPPType& Value)
        {
            void* PropertyPtr = (uint8*)OwnerPtr + this->Offset;

            TPropertyTypeLayout<TCPPType>::SetPropertyValue(PropertyPtr, Value);
        }
    };


    template<typename TCPPType>
    requires std::is_arithmetic_v<TCPPType>
    class TProperty_Numeric : public TProperty<FNumericProperty, TCPPType>
    {
    public:

        using Super = TProperty<FNumericProperty, TCPPType>;

        TProperty_Numeric(FFieldOwner InOwner, const FPropertyParams* Params)
            :Super(InOwner, Params)
        {}


    };


    


    //-------------------------------------------------------------------------------

    class FBoolProperty : public TProperty_Numeric<bool>
    {
    public:
        using Super = TProperty_Numeric<bool>;

        FBoolProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };
    
    class FInt8Property : public TProperty_Numeric<int8>
    {
    public:
        using Super = TProperty_Numeric<int8>;

        FInt8Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FInt16Property : public TProperty_Numeric<int16>
    {
    public:
        using Super = TProperty_Numeric<int16>;

        FInt16Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FInt32Property : public TProperty_Numeric<int32>
    {
    public:
        using Super = TProperty_Numeric<int32>;

        FInt32Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FInt64Property : public TProperty_Numeric<int64>
    {
    public:
        using Super = TProperty_Numeric<int64>;

        FInt64Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FUInt8Property : public TProperty_Numeric<uint8>
    {
    public:
        using Super = TProperty_Numeric<uint8>;

        FUInt8Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FUInt16Property : public TProperty_Numeric<uint16>
    {
    public:
        using Super = TProperty_Numeric<uint16>;

        FUInt16Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FUInt32Property : public TProperty_Numeric<uint32>
    {
    public:
        using Super = TProperty_Numeric<uint32>;

        FUInt32Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FUInt64Property : public TProperty_Numeric<uint64>
    {
    public:
        using Super = TProperty_Numeric<uint64>;

        FUInt64Property(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FFloatProperty : public TProperty_Numeric<float>
    {
    public:
        using Super = TProperty_Numeric<float>;

        FFloatProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    class FDoubleProperty : public TProperty_Numeric<double>
    {
    public:
        using Super = TProperty_Numeric<double>;

        FDoubleProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            : Super(InOwner, Params)
        {}
    };

    

}
