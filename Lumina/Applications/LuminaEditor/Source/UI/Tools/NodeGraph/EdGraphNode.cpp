#include "EdGraphNode.h"

#include "EdNodeGraphPin.h"
#include "Core/Math/Hash/Hash.h"
#include "Material/MaterialGraphTypes.h"

namespace Lumina
{

    static uint16 HashPinID(const FString& NodeName, const FString& PinName, ENodePinDirection Direction)
    {
        FString Composite = NodeName + "_" + PinName + "_" + eastl::to_string((uint8)Direction);
        return (uint16)Hash::GetHash32(Composite);
    }
    
    void CEdGraphNode::PostCreateCDO()
    {
        CObject::PostCreateCDO();
    }

    CEdGraphNode::~CEdGraphNode()
    {
        for (auto& PinVector : NodePins)
        {
            for (CEdNodeGraphPin* Pin : PinVector)
            {
                Pin->MarkGarbage();
            }
        }
    }

    void CEdGraphNode::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);

        for (auto& PinVector : NodePins)
        {
            for (CEdNodeGraphPin* Pin : PinVector)
            {
                Pin->Serialize(Ar);
            }
        }
    }
    
    CEdNodeGraphPin* CEdGraphNode::GetPin(uint16 ID, ENodePinDirection Direction)
    {
        for (CEdNodeGraphPin* Pin : NodePins[uint32(Direction)])
        {
            if (Pin->PinID == ID)
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
        NewPin->PinID = HashPinID(FullName, Name, Direction);
        NewPin->bInputPin = (Direction == ENodePinDirection::Input);
        NewPin->OwningNode = this;
        
        NodePins[uint32(Direction)].push_back(NewPin);

        return NewPin;
    }
}
