#pragma once

#include "Containers/Array.h"


namespace Lumina
{
    struct FField;
}

namespace Lumina
{
    class CObject;

    class Class
    {
    public:
        
        
        FString     ClassName;
        uint64      ID;
        CObject     *(*CreateFunc)() = nullptr;
        Class*      ParentClass = nullptr;
    };

    struct FClassMemberData
    {
        FField*     Fields;
        uint32      NumFields;
    };
    

    class FClassRegistry
    {
    public:
    
        static FClassRegistry& GetInstance()
        {
            static FClassRegistry Instance;
            return Instance;
        }

        void Register(Class* Class)
        {
            Registry[Class->ID] = Class;
        }

        CObject* CreateInstance(uint64 ID)
        {
            auto it = Registry.find(ID);
            if (it != Registry.end())
            {
                return it->second->CreateFunc();
            }
            return nullptr;
        }

    private:
        
        THashMap<uint64, Class*> Registry;
    };
    
    template<typename T>
    class ClassRegistryHelper
    {
    public:

        ClassRegistryHelper(CObject* (*CreateFunc)())
        {
            Class* Class = T::StaticClass();
            Class->CreateFunc = CreateFunc;
            Class->ID = typeid(T).hash_code();
            Class->ClassName = typeid(T).name();
            
            FClassRegistry::GetInstance().Register(Class);
        }
    };
}
