#include "NodeGraph.h"

namespace Lumina
{
    FNodeGraphPin* FNodeGraph::GetPin(const FName& PinName, ENodePinDirection Direction)
    {
        auto& Map = NodePins[uint32(Direction)];
        
        auto It = Map.find(PinName);

        if (It != Map.end())
        {
            return Map.at(PinName);
        }

        return nullptr;
    }
}
