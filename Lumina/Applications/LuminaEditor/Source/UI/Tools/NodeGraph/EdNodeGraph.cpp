#include "EdNodeGraph.h"

#include "EdGraphNode.h"
#include "EdNodeGraphPin.h"
#include "imnodes/imnodes.h"
#include "Core/Math/Math.h"
#include "Core/Object/Class.h"
#include <Core/Reflection/Type/LuminaTypes.h>

#include "Core/Object/Cast.h"
#include "Core/Profiler/Profile.h"

#define SHOW_DEBUG 0

namespace Lumina
{
    
    uint16 GNodeID = 0;
    
    CEdNodeGraph::CEdNodeGraph()
    {
    }
    
    CEdNodeGraph::~CEdNodeGraph()
    {
    }

    void CEdNodeGraph::Initialize()
    {
        ImNodesContext = ImNodes::EditorContextCreate();
    
        ImNodesIO& io = ImNodes::GetIO();
        io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
        io.MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyCtrl;
    
        ImNodesStyle& style = ImNodes::GetStyle();
        style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
    
        // Node appearance
        style.NodeCornerRounding = 5.0f; // Rounded corners for nodes
        style.NodePadding = ImVec2(10.0f, 10.0f); // Padding inside nodes
        style.NodeBorderThickness = 2.0f; // Thinner node borders
    
        // Link appearance
        style.LinkThickness = 2.0f; // Thinner links
        style.LinkLineSegmentsPerLength = 1.0f;
        style.LinkHoverDistance = 4.0f;
    
        // Pin appearance
        style.PinCircleRadius = 6.0f; // Larger pin circles
        style.PinQuadSideLength = 8.0f; // Larger pin quads
        style.PinTriangleSideLength = 8.0f; // Larger pin triangles
        style.PinLineThickness = 1.5f; // Thinner pin lines
        style.PinHoverRadius = 8.0f; // Larger hover area for pins
        style.PinOffset = 10.0f; // Offset pins further from node edges
    
        // Mini-map settings
        style.MiniMapPadding = ImVec2(5.0f, 5.0f);
        style.MiniMapOffset = ImVec2(15.0f, 15.0f);
    
        style.Colors[ImNodesCol_NodeBackground] = IM_COL32(50, 50, 50, 255);
        style.Colors[ImNodesCol_TitleBarHovered] = IM_COL32(70, 130, 180, 255); // SteelBlue
        style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(100, 149, 237, 255); // CornflowerBlue
    
    
        style.Colors[ImNodesCol_NodeOutline] = IM_COL32_BLACK;
        style.Colors[ImNodesCol_Link] = IM_COL32(200, 200, 200, 255);
        style.Colors[ImNodesCol_Pin] = IM_COL32(255, 255, 255, 255);
        style.Colors[ImNodesCol_PinHovered] = IM_COL32(70, 130, 180, 255);
        style.Colors[ImNodesCol_GridLine] = IM_COL32(30, 30, 30, 255);
        style.Colors[ImNodesCol_GridLinePrimary] = IM_COL32(60, 60, 60, 255);
    
        style.GridSpacing = 20.0f;
    }

    void CEdNodeGraph::Shutdown()
    {
        for (CEdGraphNode* Node : Nodes)
        {
            Node->Release();
        }

        ImNodes::EditorContextFree(ImNodesContext);
    }

    void CEdNodeGraph::Serialize(FArchive& Ar)
    {
        Super::Serialize(Ar);
    }

    void CEdNodeGraph::DrawGraph()
    {
        //ImNodes::EditorContextSet(ImNodesContext);
        LUMINA_PROFILE_SCOPE();

        TVector<CEdGraphNode*> NodesToDestroy;
        
        ImNodes::BeginNodeEditor();

        if (ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("RightClickMenu");
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
        if (ImGui::BeginPopup("RightClickMenu"))
        {
            const ImVec2 MousePos = ImGui::GetMousePosOnOpeningCurrentPopup();

            bool bWasHoveringNode = false;
            for (int i = 0; i < Nodes.size(); ++i)
            {
                CEdGraphNode* Node = Nodes[i];
                
                const int NodeID = Node->GetNodeID();
                const ImVec2 NodePos = ImNodes::GetNodeScreenSpacePos(NodeID);
                const ImVec2 NodeSize = ImNodes::GetNodeDimensions(NodeID);

                ImRect NodeRect(NodePos, NodePos + NodeSize);

                if (NodeRect.Contains(MousePos))
                {
                    if (ImGui::BeginMenu(Node->GetNodeDisplayName().c_str()))
                    {
                        if (ImGui::MenuItem("Destroy"))
                        {
                            NodesToDestroy.push_back(Node);
                        }
                        
                        ImGui::EndMenu();
                    }
                    
                    bWasHoveringNode = true;
                    break;
                }
            }

            if (!bWasHoveringNode)
            {
                if (ImGui::BeginMenu("New Node"))
                {
                    for (CClass* NodeClass : SupportedNodes)
                    {
                        CEdGraphNode* CDO = Cast<CEdGraphNode>(NodeClass->GetDefaultObject());
                        if (ImGui::MenuItem(CDO->GetNodeDisplayName().c_str()))
                        {
                            CEdGraphNode* NewNode = CreateNode(NodeClass);
                            uint64 NodeID = NewNode->NodeID;
                            ImNodes::SetNodeScreenSpacePos(NodeID, MousePos);
                            ImVec2 NodePos = ImNodes::GetNodeGridSpacePos(NodeID);
                            NewNode->SetGridPos(NodePos.x, NodePos.y);
                        }
                    }
                    ImGui::EndMenu();
                }
            }
    
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        TVector<TPair<CEdNodeGraphPin*, CEdNodeGraphPin*>> Links;
        Links.reserve(40);
        
        THashMap<uint32, CEdNodeGraphPin*> PinMap;
        PinMap.reserve(40);

        for (uint64 i = 0; i < Nodes.size(); ++i)
        {
            CEdGraphNode* Node = Nodes[i];
            
            ImNodes::PushColorStyle(ImNodesCol_TitleBar, Node->GetNodeTitleColor());

            ImNodes::BeginNode(i);
            ImNodes::BeginNodeTitleBar();

            uint64 NodeID = Node->GetNodeID();
            if (!Node->bInitialPosSet)
            {
                ImNodes::SetNodeGridSpacePos(NodeID, { Node->GetNodeX(), Node->GetNodeY() });
                Node->bInitialPosSet = true;
            }

            ImVec2 NodePos = ImNodes::GetNodeGridSpacePos(NodeID);
            Node->SetGridPos(NodePos.x, NodePos.y);

            #if SHOW_DEBUG
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s - %i", Node->GetNodeFullName().c_str(), Node->GetDebugExecutionOrder());
            ImGui::TextUnformatted(Node->GetName().c_str());
            #else
            ImGui::TextUnformatted(Node->GetNodeDisplayName().c_str());
            #endif

            ImGui::Dummy(ImVec2(Node->GetMinNodeSize().x, 0.1f));
            ImNodes::EndNodeTitleBar();

            const TVector<CEdNodeGraphPin*>& OutputPins = Node->GetOutputPins();
            for (uint64 j = 0; j < OutputPins.size(); ++j)
            {
                CEdNodeGraphPin* Pin = OutputPins[j];
    
                ImNodes::PushColorStyle(ImNodesCol_Pin, Pin->GetPinColor());
    
                ImNodesPinShape Shape = (Pin->HasConnection()) ? ImNodesPinShape_QuadFilled : ImNodesPinShape_Quad;
    
                ImNodes::BeginOutputAttribute(Pin->GetGUID(), Shape);
                
                ImGui::TextUnformatted(Pin->GetPinName().c_str());

                ImGui::SameLine();
            
                if (Pin->HasConnection() && Pin->ShouldHideDuringConnection())
                {
                    ImGui::Dummy(ImVec2(0.0f, 1.0f));
                }
                else
                {
                    float DrawWidth = Pin->DrawPin();
                }
    
                ImNodes::EndOutputAttribute();
    
                ImNodes::PopColorStyle();
    
                ImGui::Spacing();
    
                PinMap.insert_or_assign(Pin->GetGUID(), Pin);
            }

            const TVector<CEdNodeGraphPin*>& InputPins = Node->GetInputPins();
            for (uint64 j = 0; j < InputPins.size(); ++j)
            {
                CEdNodeGraphPin* Pin = InputPins[j];
                
                ImNodes::PushColorStyle(ImNodesCol_Pin, Pin->GetPinColor());
                
                ImNodesPinShape Shape = (Pin->HasConnection()) ? ImNodesPinShape_CircleFilled : ImNodesPinShape_Circle;
                
                ImNodes::BeginInputAttribute(Pin->GetGUID(), Shape);

                ImGui::TextUnformatted(Pin->GetPinName().c_str());
                
                ImGui::SameLine();
                
                if (Pin->HasConnection() && Pin->ShouldHideDuringConnection())
                {
                    ImGui::Dummy(ImVec2(1.0f, 1.0f));
                }
                else
                {
                    Pin->DrawPin();
                }
                
                ImNodes::EndInputAttribute();
                
                ImNodes::PopColorStyle();
                
                ImGui::Spacing();
                
                PinMap.insert_or_assign(Pin->GetGUID(), Pin);
            }
            
            for (uint64 j = 0; j < InputPins.size(); ++j)
            {
                CEdNodeGraphPin* InputPin = InputPins[j];
                
                Assert(InputPin->GetConnections().size() <= 1)

                for (CEdNodeGraphPin* Connection : InputPin->GetConnections())
                {
                    Links.emplace_back(TPair(InputPin, Connection));
                }
            }

            if (Node->HasError())
            {
                FString ErrorString("ERROR: " + Node->GetError());
    
                ImVec2 textPos = ImGui::GetCursorScreenPos();
                ImVec2 textSize = ImGui::CalcTextSize(ErrorString.c_str());

                float padding = 4.0f;
                ImVec2 bgMin(textPos.x - padding, textPos.y - padding);
                ImVec2 bgMax(textPos.x + textSize.x + padding, textPos.y + textSize.y + padding);

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(bgMin, bgMax, IM_COL32(50, 0, 0, 200), 4.0f);

                // Draw the text over the background
                ImGui::TextColored(ImVec4(1, 1, 1, 1), ErrorString.c_str());
            }

            
            ImNodes::EndNode();
            ImNodes::PopColorStyle();

        }

        for (int i = 0; i < Links.size(); ++i)
        {
            const TPair<CEdNodeGraphPin*, CEdNodeGraphPin*>& Pair = Links[i];
        
            ImNodes::Link(i, Pair.first->GetGUID(), Pair.second->GetGUID());
        }
    
        ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
        ImNodes::EndNodeEditor();
        
        int Start, End;
        if (ImNodes::IsLinkCreated(&Start, &End))
        {
            CEdNodeGraphPin* StartPin = nullptr;
            CEdNodeGraphPin* EndPin = nullptr;
    
            for (CEdGraphNode* Node : Nodes)
            {
                StartPin = Node->GetPin(Start, ENodePinDirection::Output);
                if (StartPin)
                {
                    break;
                }
            }

            for (CEdGraphNode* Node : Nodes)
            {
                EndPin = Node->GetPin(End, ENodePinDirection::Input);
                if (EndPin)
                {
                    break;
                }
            }

            if (!StartPin || !EndPin || StartPin == EndPin || StartPin->OwningNode == EndPin->OwningNode)
            {
                return;
            }

            if (EndPin->HasConnection())
            {
                return; // Disallow connection if the input pin is already occupied
            }

            // Allow the connection
            StartPin->AddConnection(EndPin);
            EndPin->AddConnection(StartPin);
            
            ValidateGraph();
        }


        {
            int ID;
            if (ImNodes::IsLinkDestroyed(&ID))
            {
                const TPair<CEdNodeGraphPin*, CEdNodeGraphPin*>& Pair = Links[ID];

                Pair.first->RemoveConnection(Pair.second);
                Pair.second->RemoveConnection(Pair.first);

                ValidateGraph();
            }
        }

        for (CEdGraphNode* ToDestroy : NodesToDestroy)
        {
            ToDestroy->MarkGarbage();

            auto it = eastl::find(Nodes.begin(), Nodes.end(), ToDestroy);
            if (it != Nodes.end())
            {
                Nodes.erase(it);
            }

        }
        if (!NodesToDestroy.empty())
        {
            ValidateGraph();
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
    

    uint64 CEdNodeGraph::AddNode(CEdGraphNode* InNode)
    {
        InNode->AddRef();
        SIZE_T NewID = Nodes.size();
        InNode->FullName = InNode->GetNodeDisplayName() + "_" + eastl::to_string(NewID);
        InNode->NodeID = NewID;

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

#undef SHOW_DEBUG