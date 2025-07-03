#pragma once
#include "imgui.h"
#include "Core/Object/Field.h"
#include "Core/Serialization/Structured/StructuredArchive.h"
#include "Core/Templates/Align.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FPropertyParams;
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

        LUMINA_API SIZE_T GetElementSize() const { return ElementSize; }
        LUMINA_API void SetElementSize(SIZE_T Size, SIZE_T Align) { ElementSize = Lumina::Align(ElementSize, Align); }

        template<typename ValueType>
        ValueType* SetValuePtr(void* ContainerPtr, const ValueType& Value, int32 ArrayIndex = 0)
        {
            if (sizeof(Value) != ElementSize)
            {
                return nullptr;
            }
            
            ValueType* ValuePtr = GetValuePtr<ValueType>(ContainerPtr, ArrayIndex);
            *ValuePtr = Value;
            return ValuePtr;
        }

        /** Gets the casted internal value type by an offset, UB if type is not correct */
        template<typename ValueType>
        requires !eastl::is_pointer_v<ValueType>
        ValueType* GetValuePtr(void* ContainerPtr, int32 ArrayIndex = 0) const
        {
            return (ValueType*)GetValuePtrInternal(ContainerPtr, ArrayIndex);
        }

        /** Gets the casted internal value type by an offset, UB if type is not correct */
        template<typename ValueType>
        requires !eastl::is_pointer_v<ValueType>
        const ValueType* GetValuePtr(const void* ContainerPtr, int32 ArrayIndex = 0) const
        {
            return (ValueType*)GetValuePtrInternal(const_cast<void*>(ContainerPtr), ArrayIndex);
        }

        virtual void Serialize(FArchive& Ar, void* Value) { }
        virtual void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults = nullptr) { }

        virtual void DrawProperty(void* ValuePtr) { }

    private:

        LUMINA_API void* GetValuePtrInternal(void* ContainerPtr, int32 ArrayIndex) const;
        
    public:
        
        SIZE_T ElementSize;

        /** Linked list of properties from most-derived to base */
        FProperty* PropertyLinkNext;
        
    };

    class FNumericProperty : public FProperty
    {
    public:

        FNumericProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {}

        LUMINA_API virtual void SetIntPropertyValue(void* Data, uint64 Value) const { }
        LUMINA_API virtual void SetIntPropertyValue(void* Data, int64 Value) const { }
        
        LUMINA_API virtual int64 GetSignedIntPropertyValue(void const* Data) const { return 0; }
        LUMINA_API virtual int64 GetSignedIntPropertyValue_InContainer(void const* Container) const { return 0; }
        
        LUMINA_API virtual uint64 GetUnsignedIntPropertyValue(void const* Data) const { return 0; }
        LUMINA_API virtual uint64 GetUnsignedIntPropertyValue_InContainer(void const* Container) const { return 0; }
        
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

        /** Get the value of the property from an address */
        static FORCEINLINE TCPPType const& GetPropertyValue(void const* A)
        {
            return *GetPropertyValuePtr(A);
        }

        /** Set the value of a property at an address */
        static FORCEINLINE void SetPropertyValue(void* Ptr, const TCPPType& Value)
        {
            *GetPropertyValuePtr(Ptr) = Value;
        }
        
    };
    
    template<typename TBacking, typename TCPPType>
    class TProperty : public TBacking
    {
    public:

        using TTypeInfo = TPropertyTypeLayout<TCPPType>;
        
        TProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :TBacking(InOwner, Params)
        {
            this->ElementSize = TTypeInfo::Size;
        }

        virtual void Serialize(FArchive& Ar, void* Value) override
        {
            Ar << *TTypeInfo::GetPropertyValuePtr(Value);
        }
        
        virtual void SerializeItem(IStructuredArchive::FSlot Slot, void* Value, void const* Defaults = nullptr) override
        {
            Slot.Serialize(*TTypeInfo::GetPropertyValuePtr(Value));
        }

        void DrawProperty(void* Object) override;
        
    };


    template<typename TCPPType>
    requires std::is_arithmetic_v<TCPPType>
    class TProperty_Numeric : public TProperty<FNumericProperty, TCPPType>
    {
    public:
        
        using TTypeInfo = TPropertyTypeLayout<TCPPType>;
        using Super = TProperty<FNumericProperty, TCPPType>;

        TProperty_Numeric(FFieldOwner InOwner, const FPropertyParams* Params)
            :Super(InOwner, Params)
        {}

        virtual void SetIntPropertyValue(void* Data, uint64 Value) const override;
        virtual void SetIntPropertyValue(void* Data, int64 Value) const override;

        
        
        virtual int64 GetSignedIntPropertyValue(void const* Data) const override;
        virtual int64 GetSignedIntPropertyValue_InContainer(void const* Container) const override;
        
        virtual uint64 GetUnsignedIntPropertyValue(void const* Data) const override;
        virtual uint64 GetUnsignedIntPropertyValue_InContainer(void const* Container) const override;

        
    };

    template <typename TBacking, typename TCPPType>
    void TProperty<TBacking, TCPPType>::DrawProperty(void* Object)
    {
        ImGui::PushID(this);
        TCPPType* Type = (TCPPType*)Object;

        ImGui::Text("%i", *Type);

        ImGui::PopID();
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    void TProperty_Numeric<TCPPType>::SetIntPropertyValue(void* Data, uint64 Value) const
    {
        TTypeInfo::SetPropertyValue(Data, (TCPPType)Value); 
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    void TProperty_Numeric<TCPPType>::SetIntPropertyValue(void* Data, int64 Value) const
    {
        TTypeInfo::SetPropertyValue(Data, (TCPPType)Value); 
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    int64 TProperty_Numeric<TCPPType>::GetSignedIntPropertyValue(void const* Data) const
    {
        return (int64)TTypeInfo::GetPropertyValue(Data);
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    int64 TProperty_Numeric<TCPPType>::GetSignedIntPropertyValue_InContainer(void const* Container) const
    {
        return (int64)TTypeInfo::GetPropertyValue(Container);
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    uint64 TProperty_Numeric<TCPPType>::GetUnsignedIntPropertyValue(void const* Data) const
    {
        return (uint64)TTypeInfo::GetPropertyValue(Data);
    }

    template <typename TCPPType> requires std::is_arithmetic_v<TCPPType>
    uint64 TProperty_Numeric<TCPPType>::GetUnsignedIntPropertyValue_InContainer(void const* Container) const
    {
        return (uint64)TTypeInfo::GetPropertyValue(Container);
    }


    


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
