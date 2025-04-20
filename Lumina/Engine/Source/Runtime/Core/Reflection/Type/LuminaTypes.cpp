#include "LuminaTypes.h"

namespace Lumina
{
    void FProperty::Init()
    {
        eastl::visit([this](auto& Value)
        {
            Value.AddProperty(this);
        }, Owner.Variant);
    }
}
