#include "EdNodeGraphPin.h"

namespace Lumina
{
    void CEdNodeGraphPin::RemoveConnection(CEdNodeGraphPin* Pin)
    {
        VectorRemove(Connections, Pin);
    }

}