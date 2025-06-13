#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{

    
    class FArrayProperty : public FProperty
    {
    public:

        
        FArrayProperty(FFieldOwner InOwner, const FPropertyParams* Params)
            :FProperty(InOwner, Params)
        {
        }

        ~FArrayProperty() override;

        void AddProperty(FProperty* Property) override { Inner = Property; }

        FProperty* GetInternalProperty() const { return Inner; }
    
    private:

        FProperty* Inner = nullptr;
    
    };

    template<typename T>
    class FReflectArrayHelper
    {
    public:
        FReflectArrayHelper(FArrayProperty* InArrayProperty, void* ContainerPtr)
            : Property(InArrayProperty)
        {
            TVectorPtr = (TVector<T>*)Property->GetValuePtr<void>(ContainerPtr);
        }

        T* GetData() const
        {
            return TVectorPtr->data();
        }

        TVector<T>* GetVectorPointer() const
        {
            return TVectorPtr;
        }

        int32 Num() const
        {
            return TVectorPtr->size();
        }

        T GetAt(int32 Index) const
        {
            if (Index >= 0 && Index < Num())
            {
                return GetData()[Index];
            }
            return T();
        }

        T* begin() const
        {
            return TVectorPtr->begin();
        }

        T* end() const
        {
            return TVectorPtr->end();
        }


    private:
        
        TVector<T>*     TVectorPtr;
        FArrayProperty* Property;
    };
    
}
