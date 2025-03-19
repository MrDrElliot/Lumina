#include "EdNodeGraphPin.h"

namespace Lumina
{
    void FEdNodeGraphPin::RemoveConnection(FEdNodeGraphPin* Pin)
    {
        for (int i = Connections.size() - 1; i >= 0; --i)
        {
            FEdNodeGraphPin* ConnectionPin = Connections[i];
            if (ConnectionPin == Pin)
            {
                Connections.erase(Connections.begin() + i);
            }
        }

    }
}