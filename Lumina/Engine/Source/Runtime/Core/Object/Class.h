#pragma once

#include "Lumina.h"
#include "Module/Api.h"
#include "Object.h"
#include "Core/Functional/Function.h"
#include "Initializer/ObjectInitializer.h"

namespace Lumina
{
    class FProperty;
}

namespace Lumina
{

    //--------------------------------------------------------------------------------
    // CField
    //--------------------------------------------------------------------------------
    
    class CField : public CObject
    {
    public:

        DECLARE_CLASS(Lumina, CField, CObject, "script://Lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CField)
        
        CField() = default;
        
        
        CField(const TCHAR* Package, FName InName, uint32 InSize, uint32 InAlignment, EObjectFlags InFlags)
            : CObject(nullptr, InFlags, Package, InName)
            , Size(InSize)
            , Alignment(InAlignment)
        {}
        //~ End Internal Use Only Constructors

        FProperty* LinkedProperty = nullptr;
        
        uint32 Size = 0;
        uint32 Alignment = 0;
    };


    //--------------------------------------------------------------------------------
    // CEnum
    //--------------------------------------------------------------------------------
    
    class CEnum : public CField
    {
    public:
        
        DECLARE_CLASS(Lumina, CEnum, CField, "script://lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CEnum)

        CEnum()
        {}

        LUMINA_API FName GetNameAtValue(uint64 Value);
        LUMINA_API uint64 GetEnumValueByName(FName Name);
        void AddEnum(FName Name, uint64 Value);

        void ForEachEnum(TFunction<void(const TPair<FName, uint64>&)> Functor);

        TVector<TPair<FName, uint64>> Names;

        

    };
    

    //--------------------------------------------------------------------------------
    // CStruct
    //--------------------------------------------------------------------------------
    
    
    /** Base class for any data structure that holds fields */
    class CStruct : public CField
    {

        DECLARE_CLASS(Lumina, CStruct, CField, "script://Lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CStruct)

        CStruct() = default;

        // Begin Internal Use Only Constructors 
        CStruct(const TCHAR* Package, FName InName, uint32 InSize, uint32 InAlignment, EObjectFlags InFlags)
            : CField(Package, InName, InSize, InAlignment, InFlags)
        {}
        //~ End Internal Use Only Constructors

        
        virtual void SetSuperStruct(CStruct* InSuper);

        void RegisterDependencies() override;
        
        FORCEINLINE uint32 GetSize() const { return Size; }
        FORCEINLINE uint32 GetAlignment() const { return Alignment; }

        /** Struct this inherits from, may be null */
        CStruct* GetSuperStruct() const { return SuperStruct; }

        LUMINA_API FProperty* GetProperty(const FName& Name);
        LUMINA_API virtual void AddProperty(FProperty* Property);

        template<typename PropertyType>
        PropertyType* GetProperty(const FName& Name)
        {
            return (PropertyType*)GetProperty(Name);
        }

        template<typename PropertyType>
        requires std::is_base_of_v<FProperty, PropertyType>
        void ForEachProperty(TMoveOnlyFunction<void(PropertyType*)> Callback)
        {
            PropertyType* Current = (PropertyType*)LinkedProperty;
            while (Current != nullptr)
            {
                if (typeid(PropertyType) == typeid(*Current))
                {
                    Callback(Current);
                }
                Current = (PropertyType*)Current->Next;
            }
        }

        LUMINA_API void ForEachProperty(TMoveOnlyFunction<void(FProperty*)> Callback);

        template<class T>
        bool IsChildOf() const
        {
            return IsChildOf(T::StaticClass());
        }
        
        LUMINA_API bool IsChildOf(const CStruct* Base) const;

        /** Links a derived to it's parent (if one exists) and will link properties.. */
        LUMINA_API virtual void Link();
        
    private:

        /** Parent struct */
        CStruct* SuperStruct = nullptr;
        
        bool bLinked = false;
    };



    //--------------------------------------------------------------------------------
    // CClass
    //--------------------------------------------------------------------------------
    
    
    /** Final class for fields and functions. */
    class CClass final : public CStruct
    {
    public:

        DECLARE_CLASS(Lumina, CClass, CStruct, "script://Lumina", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CClass)

        typedef void (*ClassConstructorType) (const FObjectInitializer&);
        
        ClassConstructorType ClassConstructor;
        
        CClass() = default;

        // Begin Internal Use Only Constructors 
        CClass(const TCHAR* Package, FName InName, uint32 InSize, uint32 InAlignment, EObjectFlags InFlags, ClassConstructorType InConstructor)
            : CStruct(Package, InName, InSize, InAlignment, InFlags)
            , ClassConstructor(InConstructor)
        {}
        //~ End Internal Use Only Constructors

        LUMINA_API CObject* GetDefaultObject() const
        {
            if (ClassDefaultObject == nullptr)
            {
                CClass* MutableThis = const_cast<CClass*>(this);
                MutableThis->CreateDefaultObject();
            }

            return ClassDefaultObject;
        }

        template<typename T>
        T* GetDefaultObject() const
        {
            return static_cast<T*>(GetDefaultObject());
        }


        mutable int32   ClassUnique = 0;

        
    protected:

        LUMINA_API CObject* CreateDefaultObject();

    private:

        CObject*        ClassDefaultObject = nullptr;

    };

    template<class T>
    void InternalConstructor(const FObjectInitializer& IO)
    { 
        T::__DefaultConstructor(IO);
    }

    LUMINA_API void AllocateStaticClass(const TCHAR* Package, const TCHAR* Name, CClass** OutClass, uint32 Size, uint32 Alignment, CClass* (*SuperClassFn)(), CClass::ClassConstructorType InClassConstructor);


}
