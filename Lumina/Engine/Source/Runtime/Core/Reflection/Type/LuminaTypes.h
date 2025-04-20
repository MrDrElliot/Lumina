#pragma once
#include "Core/Object/Field.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    class FProperty : public FField
    {
    public:

        FProperty(FFieldOwner InOwner)
            :FField(InOwner)
        {
            Init();
        }
        

        void Init();

        void SetValue(void* Data, uint32 Value);
        
        uint32 Size;
        uint32 ElementSize;
        
    };

    class FNumericProperty : public FProperty
    {
    public:

        FNumericProperty(FFieldOwner InOwner)
            :FProperty(InOwner)
        {}
        
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

        TProperty(FFieldOwner InOwner)
            :TBacking(InOwner)
        {
            this->ElementSize = TPropertyTypeLayout<TCPPType>::Size;
        }
        
    };


    template<typename TCPPType>
    requires std::is_integral_v<TCPPType>
    class TProperty_Numeric : public TProperty<FNumericProperty, TCPPType>
    {
    public:

        using Super = TProperty<FNumericProperty, TCPPType>;

        TProperty_Numeric(FFieldOwner InOwner)
            :Super(InOwner)
        {}
        
        
    };


    //-------------------------------------------------------------------------------
    
    
    class FUInt8Property : public TProperty_Numeric<uint8>
    {
    public:

        using Super = TProperty_Numeric<uint8>;

        FUInt8Property(FFieldOwner InOwner)
            :Super(InOwner)
        {}
        
    };
    

}
