#include "EdGraphNode.h"

#include "EdNodeGraphPin.h"
#include "Material/MaterialGraphTypes.h"

namespace Lumina
{
    void CEdGraphNode::PostCreateCDO()
    {
        CObject::PostCreateCDO();
    }

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
        return NodePins[uint32(Direction)][Index];
    }

    CEdNodeGraphPin* CEdGraphNode::CreatePin(CClass* InClass, const FString& Name, ENodePinDirection Direction, EMaterialInputType Type)
    {
        CEdNodeGraphPin* NewPin = NewObject<CEdNodeGraphPin>(InClass);
        NewPin->GUID = Math::RandRange<uint16>(0, UINT16_MAX);
        NewPin->bInputPin = Direction == ENodePinDirection::Input;
        NewPin->OwningNode = this;
        
        NodePins[uint32(Direction)].push_back(NewPin);

        return NewPin;
    }
}
