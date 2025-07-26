#include "ComponentRegistry.h"

#include <utility>

#include "Containers/Array.h"
#include "Core/Object/Class.h"

namespace Lumina
{
    struct FEntityComponentAdd
    {
        const char* Name;
        Components::ComponentAddFn Fn;
    };
    static TFixedVector<FEntityComponentAdd, 2024> ComponentToAdd;

    void Components::RegisterComponent(const char* Struct, ComponentAddFn Fn)
    {
        FEntityComponentAdd Add;
        Add.Name = Struct;
        Add.Fn = std::move(Fn);
        ComponentToAdd.push_back(Add);
    }

    Components::ComponentAddFn Components::GetEntityComponentCreationFn(CStruct* Struct)
    {
        for (const FEntityComponentAdd& Add : ComponentToAdd)
        {
            if (std::strcmp(Add.Name, Struct->GetName().c_str()) == 0)
            {
                return Add.Fn;
            }
        }

        return nullptr;
    }
}
