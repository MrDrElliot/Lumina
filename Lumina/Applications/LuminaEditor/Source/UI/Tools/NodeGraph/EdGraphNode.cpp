#include "EdGraphNode.h"

#include "EdNodeGraphPin.h"
#include "Core/Math/Hash/Hash.h"
#include "Material/MaterialGraphTypes.h"
#include "imgui-node-editor/imgui_node_editor.h"

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

    void CEdGraphNode::PushNodeStyle()
    {
        using namespace ax;

        NodeEditor::PushStyleColor(NodeEditor::StyleColor_NodeBg,        ImColor(128, 128, 128, 200));
        NodeEditor::PushStyleColor(NodeEditor::StyleColor_NodeBorder,    ImColor( 32,  32,  32, 200));
        NodeEditor::PushStyleColor(NodeEditor::StyleColor_PinRect,       ImColor( 60, 180, 255, 150));
        NodeEditor::PushStyleColor(NodeEditor::StyleColor_PinRectBorder, ImColor( 60, 180, 255, 150));

        NodeEditor::PushStyleVar(NodeEditor::StyleVar_NodePadding,  ImVec4(0, 0, 0, 0));
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_NodeRounding, 10);
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f,  1.0f));
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_LinkStrength, 0.0f);
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_PinBorderWidth, 1.0f);
        NodeEditor::PushStyleVar(NodeEditor::StyleVar_PinRadius, 5.0f);
    }

    void CEdGraphNode::PopNodeStyle()
    {
        using namespace ax;

        NodeEditor::PopStyleColor(4);
        NodeEditor::PopStyleVar(7);
    }

    void CEdGraphNode::DrawNodeTitleBar()
    {
        ImGui::TextUnformatted(GetNodeDisplayName().c_str());
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
