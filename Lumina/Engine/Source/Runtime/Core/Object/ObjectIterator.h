#pragma once
#include "ObjectArray.h"
#include "ObjectBase.h"
#include "Containers/Array.h"

namespace Lumina
{
    class CObjectBase;

    template<typename T>
    requires !std::is_same_v<T, CObjectBase>
    class TObjectIterator
    {
    public:
        using ValueType = T*;

        explicit TObjectIterator(bool bIncludeDerivedClasses = true)
            : Index(0)
        {
            for (FCObjectArray::FEntry& Entry : GObjectArray.Objects)
            {
                CObjectBase* Obj = Entry.Object;
                
                if (!Obj) continue;

                if (bIncludeDerivedClasses)
                {
                    if (Obj->IsA<T>())
                    {
                        AllObjects.push_back(Obj);
                    }
                }
                else
                {
                    if (typeid(*Obj) == typeid(T))
                    {
                        AllObjects.push_back(Obj);
                    }
                }
            }

            AdvanceToNextValid();
        }
        
        TObjectIterator& operator++()
        {
            ++Index;
            AdvanceToNextValid();
            return *this;
        }

        ValueType operator*() const
        {
            return static_cast<T*>(AllObjects[Index]);
        }

        explicit operator bool() const
        {
            return Index < AllObjects.size();
        }

    private:
        void AdvanceToNextValid()
        {
            while (Index < AllObjects.size() && AllObjects[Index] == nullptr)
            {
                ++Index;
            }
        }

        TFixedVector<CObjectBase*, 2024> AllObjects;
        SIZE_T Index;
    };
}

