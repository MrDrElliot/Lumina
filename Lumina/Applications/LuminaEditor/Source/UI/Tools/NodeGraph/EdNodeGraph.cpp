#include "EdNodeGraph.h"

#include "EdGraphNode.h"
#include "Core/Object/Class.h"
#include <Core/Reflection/Type/LuminaTypes.h>

#include "Drawing.h"
#include "imgui_internal.h"
#include "Core/Object/Cast.h"
#include "Core/Profiler/Profile.h"
#include "imgui-node-editor/imgui_node_editor_internal.h"
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    static void DrawPinIcon(bool bConnected, int Alpha, ImVec4 Color)
    {
        EIconType iconType = EIconType::Circle;
        Color.w = Alpha / 255.0f;
        
        Icon(ImVec2(24.f, 24.0f), iconType, bConnected, Color, ImColor(32, 32, 32, Alpha));
    }

    static ImRect ImGui_GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }
    
    uint16 GNodeID = 0;
    
    CEdNodeGraph::CEdNodeGraph()
        : NodeSelectedCallback()
    {
    }

    CEdNodeGraph::~CEdNodeGraph()
    {
    }

    void CEdNodeGraph::Initialize()
    {
        ax::NodeEditor::Config config;
        config.SettingsFile = "Simple.json";
        Context = ax::NodeEditor::CreateEditor(&config);
        
    }

    void CEdNodeGraph::Shutdown()
    {
        ax::NodeEditor::DestroyEditor(Context);
        Context = nullptr;
    }

    void CEdNodeGraph::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
    }

    void CEdNodeGraph::DrawGraph()
    {
        LUMINA_PROFILE_SCOPE();

        using namespace ax;
        
        NodeEditor::SetCurrentEditor(Context);

        NodeEditor::Begin("Node Editor");

        Graph::GraphNodeBuilder NodeBuilder;

        TVector<TPair<CEdNodeGraphPin*, CEdNodeGraphPin*>> Links;
        Links.reserve(40);
        
        for (CEdGraphNode* Node : Nodes)
        {
            NodeBuilder.Begin(Node->GetNodeID());

            if (!Node->WantsTitlebar())
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
            }
            
            NodeBuilder.Header(ImGui::ColorConvertU32ToFloat4(Node->GetNodeTitleColor()));
            
            if (!Node->WantsTitlebar())
            {
                ImGui::PopStyleVar();
            }
            
            ImGui::Spring(0);
            Node->DrawNodeTitleBar();
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(Node->GetMinNodeTitleBarSize()));
            ImGui::Spring(0);
            NodeBuilder.EndHeader();

            if (Node->GetInputPins().empty())
            {
                ImGui::BeginVertical("inputs", ImVec2(0,0), 0.0f);
                ImGui::Dummy(ImVec2(0,0));
                ImGui::EndVertical();
            }
            
            for (CEdNodeGraphPin* InputPin : Node->GetInputPins())
            {
                for (CEdNodeGraphPin* Connection : InputPin->GetConnections())
                {
                    Links.emplace_back(TPair(InputPin, Connection));
                }
                
                NodeBuilder.Input(InputPin->GetGUID());

                ImGui::PushID(InputPin);
                {
                    DrawPinIcon(InputPin->HasConnection(), 255.0f, ImGui::ColorConvertU32ToFloat4(InputPin->GetPinColor()));
                    ImGui::Spring(0);
                    //if (!InputPin->HasConnection() || !InputPin->ShouldHideDuringConnection())
                    //{
                    //    InputPin->DrawPin();
                    //}
                    ImGui::TextUnformatted(InputPin->GetPinName().c_str());
                    ImGui::Spring(0);
                }
                ImGui::PopID();
                
                NodeBuilder.EndInput();
            }

            NodeBuilder.Middle();
            Node->DrawNodeBody();
            
            
            for (CEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                NodeBuilder.Output(OutputPin->GetGUID());
                
                ImGui::PushID(OutputPin);
                {
                    ImGui::Spring(0);
                    //if (!OutputPin->HasConnection() || !OutputPin->ShouldHideDuringConnection())
                    //{
                    //    OutputPin->DrawPin();
                    //}
                    ImGui::Spring(1, 1);
                    ImGui::TextUnformatted(OutputPin->GetPinName().c_str());
                    ImGui::Spring(0);
                    DrawPinIcon(OutputPin->HasConnection(), 255.0f, ImGui::ColorConvertU32ToFloat4(OutputPin->GetPinColor()));
                }
                ImGui::PopID();

                NodeBuilder.EndOutput();
            }
            
            NodeBuilder.End(Node->WantsTitlebar());
        }

        NodeEditor::Suspend();
        {
            if (NodeEditor::ShowBackgroundContextMenu())
            {
                ImGui::OpenPopup("Create New Node");
            }

            if (ImGui::BeginPopup("Create New Node"))
            {
                DrawGraphContextMenu();
            
                ImGui::EndPopup();
            }
        }
        
        NodeEditor::Resume();

        bool bAnyNodeSelected = false;
        for (CEdGraphNode* Node : Nodes)
        {
            if (NodeEditor::IsNodeSelected(Node->GetNodeID()))
            {
                bAnyNodeSelected = true;
                if (NodeSelectedCallback)
                {
                    NodeSelectedCallback(Node);
                }
            }
        }

        if (!bAnyNodeSelected)
        {
            if (NodeSelectedCallback)
            {
                NodeSelectedCallback(nullptr);
            }
        }
        
        uint32 LinkID = 1;
        for (auto& [Start, End] : Links)
        {
            NodeEditor::Link(LinkID++, Start->GetGUID(), End->GetGUID());
        }
        
        if (NodeEditor::BeginCreate())
        {
            NodeEditor::PinId StartPinID, EndPinID;
            if (NodeEditor::QueryNewLink(&StartPinID, &EndPinID))
            {
                if (((StartPinID && EndPinID) && (StartPinID != EndPinID)) && NodeEditor::AcceptNewItem())
                {
                    CEdNodeGraphPin* StartPin = nullptr;
                    CEdNodeGraphPin* EndPin = nullptr;

                    for (CEdGraphNode* Node : Nodes)
                    {
                        StartPin = Node->GetPin(StartPinID.Get(), ENodePinDirection::Output);
                        if (StartPin) break;
                    }

                    for (CEdGraphNode* Node : Nodes)
                    {
                        EndPin = Node->GetPin(EndPinID.Get(), ENodePinDirection::Input);
                        if (EndPin) break;
                    }

                    bool bValid = true;

                    if (!StartPin || !EndPin || StartPin == EndPin || StartPin->OwningNode == EndPin->OwningNode)
                    {
                        bValid = false;
                    }

                    if (EndPin && EndPin->HasConnection())
                    {
                        bValid = false;
                    }

                    if (bValid)
                    {
                        StartPin->AddConnection(EndPin);
                        EndPin->AddConnection(StartPin);

                        ValidateGraph();
                    }
                }
            }
        }
        NodeEditor::EndCreate();
        
        
        if (NodeEditor::BeginDelete())
        {

            NodeEditor::NodeId NodeId = 0;
            while (NodeEditor::QueryDeletedNode(&NodeId))
            {
                if (NodeEditor::AcceptDeletedItem())
                {
                    NodesToDestroy.push(Nodes[NodeId.Get()]);
                }
            }
            
            NodeEditor::LinkId DeletedLinkId;
            while (NodeEditor::QueryDeletedLink(&DeletedLinkId))
            {
                if (NodeEditor::AcceptDeletedItem())
                {
                    const TPair<CEdNodeGraphPin*, CEdNodeGraphPin*>& Pair = Links[DeletedLinkId.Get() - 1];

                    Pair.first->RemoveConnection(Pair.second);
                    Pair.second->RemoveConnection(Pair.first);
                    ValidateGraph();
                }
            }
        }
        NodeEditor::EndDelete();


        while (!NodesToDestroy.empty())
        {
            CEdGraphNode* ToDestroy = NodesToDestroy.front();
            NodesToDestroy.pop();
            
            // Remove links from input pins
            for (CEdNodeGraphPin* Pin : ToDestroy->GetInputPins())
            {
                if (Pin->HasConnection())
                {
                    for (CEdNodeGraphPin* ConnectedPin : Pin->GetConnections())
                    {
                        ConnectedPin->DisconnectFrom(Pin);
                    }
                    Pin->ClearConnections();
                }
            }

            // Remove links from output pins
            for (CEdNodeGraphPin* Pin : ToDestroy->GetOutputPins())
            {
                if (Pin->HasConnection())
                {
                    for (CEdNodeGraphPin* ConnectedPin : Pin->GetConnections())
                    {
                        ConnectedPin->DisconnectFrom(Pin);
                    }
                    Pin->ClearConnections();
                }
            }

            CEdGraphNode* BackNode = Nodes.back();
            Nodes.erase(Nodes.begin() + ToDestroy->Index);
            BackNode->Index = ToDestroy->Index;
            BackNode->bInitialPosSet = false;
            
            ToDestroy->MarkGarbage();
            ValidateGraph();
        }
    
        NodeEditor::End();
        
        NodeEditor::SetCurrentEditor(nullptr);
    }

    void CEdNodeGraph::DrawGraphContextMenu()
    {
        ImVec2 PopupSize(200, 350);
        ImGui::SetNextWindowSize(PopupSize, ImGuiCond_Once);

        ImGuiTextFilter Filter;
        Filter.Draw("Node");
        
        THashMap<FName, TVector<CClass*>> CategoryMap;
        THashMap<FName, bool> Expanded;
        for (CClass* NodeClass : SupportedNodes)
        {
            CEdGraphNode* CDO = Cast<CEdGraphNode>(NodeClass->GetDefaultObject());
            FName Category = CDO->GetNodeCategory().c_str();
            
            if (Filter.PassFilter(CDO->GetNodeDisplayName().c_str()))
            {
                CategoryMap[Category].push_back(NodeClass);
                Expanded[Category] = true;
            }
            else
            {
                Expanded[Category] = false;
            }
        }

        ImVec2 ChildSize = ImVec2(-1, PopupSize.y - 40);
        if (ImGui::BeginChild("NodeList", ChildSize, false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            for (const auto& [Category, NodeClasses] : CategoryMap)
            {
                ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_Framed;
                if (Expanded.at(Category))
                {
                    Flags |= ImGuiTreeNodeFlags_DefaultOpen;
                }
                
                if (ImGui::CollapsingHeader(Category.c_str(), Flags))
                {
                    ImGui::Indent(10);
                
                    for (CClass* NodeClass : NodeClasses)
                    {
                        CEdGraphNode* NodeCDO = NodeClass->GetDefaultObject<CEdGraphNode>();
                        ImGui::PushID(NodeCDO);
                        if (ImGui::Selectable(NodeCDO->GetNodeDisplayName().c_str()))
                        {
                            CEdGraphNode* NewNode = CreateNode(NodeClass);
                            ax::NodeEditor::SetNodePosition(NewNode->GetNodeID(), ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePosOnOpeningCurrentPopup()));
                            ImGui::CloseCurrentPopup();
                        }

                        ImGuiX::ItemTooltip("%s", NodeCDO->GetNodeTooltip().c_str());
                        ImGui::PopID();
                    }

                    ImGui::Unindent(10);
                }
            }
            ImGui::EndChild();
        }
    }


    void CEdNodeGraph::OnDrawGraph()
    {
        
    }

    void CEdNodeGraph::RegisterGraphAction(const FString& ActionName, const TFunction<void()>& ActionCallback)
    {
        FAction NewAction;
        NewAction.ActionName = ActionName;
        NewAction.ActionCallback = ActionCallback;

        Actions.push_back(NewAction);
    }

    CEdGraphNode* CEdNodeGraph::CreateNode(CClass* NodeClass)
    {
        CEdGraphNode* NewNode = NewObject<CEdGraphNode>(NodeClass);
        AddNode(NewNode);
        return NewNode;
    }


    void CEdNodeGraph::RegisterGraphNode(CClass* InClass)
    {
        if (SupportedNodes.find(InClass) == SupportedNodes.end())
        {
            SupportedNodes.emplace(InClass);
        }
    }

    uint64 CEdNodeGraph::AddNode(CEdGraphNode* InNode)
    {
        SIZE_T NewID = Nodes.size();
        InNode->FullName = InNode->GetNodeDisplayName() + "_" + eastl::to_string(NewID);
        InNode->NodeID = NewID + 1;
        InNode->Index = NewID;

        Nodes.push_back(InNode);

        if (!InNode->bWasBuild)
        {
            InNode->BuildNode();
            InNode->bWasBuild = true;
        }
        
        ValidateGraph();

        return NewID;
    }
    
}
