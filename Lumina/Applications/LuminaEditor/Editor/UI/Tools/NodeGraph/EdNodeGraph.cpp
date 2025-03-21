﻿#include "EdNodeGraph.h"

#include "EdGraphNode.h"
#include "EdNodeGraphPin.h"
#include "imnodes.h"

#define SHOW_DEBUG 1

namespace Lumina
{
    FEdNodeGraph::FEdNodeGraph()
    {
        ImNodesContext = ImNodes::EditorContextCreate();
        //ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
    
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


    FEdNodeGraph::~FEdNodeGraph()
    {
        for (FEdGraphNode* Node : Nodes)
        {
            FMemory::Delete(Node);            
        }
        
        ImNodes::EditorContextFree(ImNodesContext);
    }

    void FEdNodeGraph::DrawGraph()
    {
        ImNodes::EditorContextSet(ImNodesContext);

        ImNodes::BeginNodeEditor();

        if (ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("RightClickMenu");
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
        if (ImGui::BeginPopup("RightClickMenu"))
        {
            const ImVec2 MousePos = ImGui::GetMousePosOnOpeningCurrentPopup();
            
            if (ImGui::BeginMenu("New Node"))
            {
                for (const auto& KVP : NodeFactories)
                {
                    if (ImGui::MenuItem(KVP.first.c_str()))
                    {
                        FEdGraphNode* NewNode = KVP.second.CreationCallback();
                        uint32 NodeID = AddNode(NewNode);
                        ImNodes::SetNodeScreenSpacePos(NodeID, MousePos);
                    }
                }
                ImGui::EndMenu();
            }
    
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        TVector<TPair<FEdNodeGraphPin*, FEdNodeGraphPin*>> Links;
        THashMap<uint32, FEdNodeGraphPin*> PinMap;
    
        for (int i = 0; i < Nodes.size(); ++i)
        {
            FEdGraphNode* Node = Nodes[i];
            
            ImNodes::PushColorStyle(ImNodesCol_TitleBar, Node->GetNodeTitleColor());

            ImNodes::BeginNode(i);
            ImNodes::BeginNodeTitleBar();
        
            ImGui::TextUnformatted(Node->GetNodeDisplayName().c_str());

            #if SHOW_DEBUG
            ImGui::SameLine();
            ImGui::Text("Order: %i", Node->GetDebugExecutionOrder());
            #endif

            ImGui::Dummy(ImVec2(Node->GetMinNodeSize().x, 0.1f));
            ImNodes::EndNodeTitleBar();

            const TVector<FEdNodeGraphPin*>& OutputPins = Node->GetOutputPins();
            for (uint64 j = 0; j < OutputPins.size(); ++j)
            {
                FEdNodeGraphPin* Pin = OutputPins[j];
    
                ImNodes::PushColorStyle(ImNodesCol_Pin, Pin->GetPinColor());
    
                ImNodesPinShape Shape = (Pin->HasConnection()) ? ImNodesPinShape_QuadFilled : ImNodesPinShape_Quad;
    
                ImNodes::BeginOutputAttribute(Pin->GUID, Shape);

                ImGui::TextUnformatted(Pin->GetPinName().c_str());
                ImGui::SameLine();
    
                Pin->DrawPin();
    
                ImNodes::EndOutputAttribute();
    
                ImNodes::PopColorStyle();
    
                ImGui::Spacing();
    
                PinMap.insert_or_assign(Pin->GUID, Pin);
            }

            const TVector<FEdNodeGraphPin*>& InputPins = Node->GetInputPins();
            for (uint64 j = 0; j < InputPins.size(); ++j)
            {
                FEdNodeGraphPin* Pin = InputPins[j];
                
                ImNodes::PushColorStyle(ImNodesCol_Pin, Pin->GetPinColor());
                
                ImNodesPinShape Shape = (Pin->HasConnection()) ? ImNodesPinShape_CircleFilled : ImNodesPinShape_Circle;
                ImNodes::BeginInputAttribute(Pin->GUID, Shape);
                
                ImGui::TextUnformatted(Pin->GetPinName().c_str());
                
                ImGui::SameLine();
                
                Pin->DrawPin();
                
                ImNodes::EndInputAttribute();
                
                ImNodes::PopColorStyle();
                
                ImGui::Spacing();
                
                PinMap.insert_or_assign(Pin->GUID, Pin);
            }
            
            for (uint64 j = 0; j < InputPins.size(); ++j)
            {
                FEdNodeGraphPin* InputPin = InputPins[j];

                for (FEdNodeGraphPin* Connection : InputPin->GetConnections())
                {
                    Links.push_back(TPair(InputPin, Connection));
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
            const TPair<FEdNodeGraphPin*, FEdNodeGraphPin*>& Pair = Links[i];
        
            ImNodes::Link(i, Pair.first->GUID, Pair.second->GUID);
        }
    
        ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
        ImNodes::EndNodeEditor();
    
        int Start, End;
        if (ImNodes::IsLinkCreated(&Start, &End))
        {
            FEdNodeGraphPin* StartPin = nullptr;
            FEdNodeGraphPin* EndPin = nullptr;
            bool bCanSafeConnect = true;
        
            for (FEdGraphNode* Node : Nodes)
            {
                StartPin = Node->GetPin(Start, ENodePinDirection::Output);
                if (StartPin != nullptr)
                {
                    break;
                }
            }
        
            for (FEdGraphNode* Node : Nodes)
            {
                EndPin = Node->GetPin(End, ENodePinDirection::Input);
                if (EndPin != nullptr)
                {
                    break;
                }
            }

            Assert(StartPin != EndPin);
            Assert(StartPin->OwningNode != EndPin->OwningNode);

            if (EndPin->bSingleInput)
            {
                if (StartPin->HasConnection())
                {
                    bCanSafeConnect = false;
                }
            }

            if (EndPin->bSingleInput)
            {
                if (EndPin->HasConnection())
                {
                    bCanSafeConnect = false;
                }
            }
            
            
            if (StartPin != nullptr && EndPin != nullptr && bCanSafeConnect)
            {
                StartPin->AddConnection(EndPin);
                EndPin->AddConnection(StartPin);
            }
        }

        {
            int ID;
            if (ImNodes::IsLinkDestroyed(&ID))
            {
                const TPair<FEdNodeGraphPin*, FEdNodeGraphPin*>& Pair = Links[ID];

                Pair.first->RemoveConnection(Pair.second);
                Pair.second->RemoveConnection(Pair.first);
            }
        }
    }


    void FEdNodeGraph::OnDrawGraph()
    {
        
    }

    void FEdNodeGraph::RegisterGraphAction(const FString& ActionName, const TFunction<void()>& ActionCallback)
    {
        FAction NewAction;
        NewAction.ActionName = ActionName;
        NewAction.ActionCallback = ActionCallback;

        Actions.push_back(NewAction);
    }

    uint32 FEdNodeGraph::AddNode(FEdGraphNode* InNode)
    {
        Nodes.push_back(InNode);
        
        InNode->BuildNode();

        return Nodes.size() - 1;
    }
}

#undef SHOW_DEBUG