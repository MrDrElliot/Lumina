#include "EdNodeGraphPin.h"

namespace Lumina
{
    void FEdNodeGraphPin::RemoveConnection(FEdNodeGraphPin* Pin)
    {
        VectorRemove(Connections, Pin);
    }

}