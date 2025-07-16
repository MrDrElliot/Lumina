#pragma once

namespace Lumina
{
    enum class EResourceStates : unsigned int;
    struct FResourceStateMapping;
}

namespace Lumina::Vk
{
    FResourceStateMapping ConvertResourceState(EResourceStates State);
}
