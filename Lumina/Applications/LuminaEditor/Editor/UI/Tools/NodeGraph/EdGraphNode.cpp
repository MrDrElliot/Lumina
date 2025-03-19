#include "EdGraphNode.h"

#include "EdNodeGraphPin.h"

namespace Lumina
{
    FEdGraphNode::~FEdGraphNode()
    {
        for (auto& Vector : NodePins)
        {
            for (FEdNodeGraphPin* Pin : Vector)
            {
               FMemory::Delete(Pin); 
            }
        }
    }

    FEdNodeGraphPin* FEdGraphNode::GetPin(uint32 ID, ENodePinDirection Direction)
    {
        for (FEdNodeGraphPin* Pin : NodePins[uint32(Direction)])
        {
            if (Pin->GUID == ID)
            {
                return Pin;
            }
        }
        
        return nullptr;
    }

    FEdNodeGraphPin* FEdGraphNode::GetPinByIndex(uint32 Index, ENodePinDirection Direction)
    {
        return  NodePins[uint32(Direction)][Index];
    }
}
