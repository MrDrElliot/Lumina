#include "EdGraphNode.h"

#include "EdNodeGraphPin.h"

namespace Lumina
{
    IMPLEMENT_CLASS(CEdGraphNode)
    
    CEdGraphNode::~CEdGraphNode()
    {
        for (auto& Vector : NodePins)
        {
            for (CEdNodeGraphPin* Pin : Vector)
            {
               FMemory::Delete(Pin); 
            }
        }
    }

    void CEdGraphNode::Serialize(FArchive& Ar)
    {
        for (const TVector<CEdNodeGraphPin*>& Vector : NodePins)
        {
            for (CEdNodeGraphPin* Pin : Vector)
            {
                Pin->Serialize(Ar);
            }
        }
    }

    CEdNodeGraphPin* CEdGraphNode::GetPin(uint32 ID, ENodePinDirection Direction)
    {
        for (CEdNodeGraphPin* Pin : NodePins[uint32(Direction)])
        {
            if (Pin->GUID == ID)
            {
                return Pin;
            }
        }
        
        return nullptr;
    }

    CEdNodeGraphPin* CEdGraphNode::GetPinByIndex(uint32 Index, ENodePinDirection Direction)
    {
        return  NodePins[uint32(Direction)][Index];
    }
}
