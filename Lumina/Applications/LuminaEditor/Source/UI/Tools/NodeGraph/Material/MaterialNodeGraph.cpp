#include "MaterialNodeGraph.h"

#include "Core/Object/Class.h"
#include "Core/Object/Cast.h"
#include "Renderer/RHIIncl.h"
#include "MaterialCompiler.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Profiler/Profile.h"
#include "Nodes/MaterialNodeExpression.h"
#include "Nodes/MaterialNodeGetTime.h"
#include "Nodes/MaterialNode_TextureSample.h"
#include "Nodes/MaterialOutputNode.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"


namespace Lumina
{
    CMaterialNodeGraph::CMaterialNodeGraph()
    {
        
    }

    void CMaterialNodeGraph::Initialize()
    {
        Super::Initialize();
        
        bool bHasOutputNode = false;
        auto NewNodes = Memory::Move(Nodes);
        auto NewConnections = Memory::Move(Connections);
        Nodes.clear();
        Connections.clear();
        
        for (CObject* Object : NewNodes)
        {
            CEdGraphNode* Node = Cast<CEdGraphNode>(Object);
            if (Node == nullptr)
            {
                continue;
            }
            
            if (Node->IsA<CMaterialOutputNode>())
            {
                bHasOutputNode = true;
            }
            
            AddNode(Node);
        }

        if (!bHasOutputNode)
        {
            CreateNode(CMaterialOutputNode::StaticClass());
        }
        
        for (SIZE_T i = 0; i < NewConnections.size(); i += 2)
        {
            uint16 FirstConnection = NewConnections[i];
            uint16 SecondConnection = NewConnections[i + 1];
            
            CEdNodeGraphPin* StartPin = nullptr;
            CEdNodeGraphPin* EndPin = nullptr;

            for (CEdGraphNode* Node : Nodes)
            {
                EndPin = Node->GetPin(FirstConnection, ENodePinDirection::Input);
                if (EndPin)
                {
                    break;
                }
            }
            
            for (CEdGraphNode* Node : Nodes)
            {
                StartPin = Node->GetPin(SecondConnection, ENodePinDirection::Output);
                if (StartPin)
                {
                    break;
                }
            }

            if (!StartPin || !EndPin || StartPin == EndPin || StartPin->OwningNode == EndPin->OwningNode)
            {
                continue;
            }

            if (EndPin->HasConnection())
            {
                continue; // Disallow connection if the input pin is already occupied
            }

            // Allow the connection
            StartPin->AddConnection(EndPin);
            EndPin->AddConnection(StartPin);
        }
        
        
        RegisterGraphNode(CMaterialExpression_Addition::StaticClass());
        RegisterGraphNode(CMaterialExpression_Subtraction::StaticClass());
        RegisterGraphNode(CMaterialExpression_Division::StaticClass());
        RegisterGraphNode(CMaterialExpression_Multiplication::StaticClass());
        RegisterGraphNode(CMaterialExpression_Sin::StaticClass());
        RegisterGraphNode(CMaterialExpression_Cosin::StaticClass());
        RegisterGraphNode(CMaterialExpression_Floor::StaticClass());
        RegisterGraphNode(CMaterialExpression_Ceil::StaticClass());
        RegisterGraphNode(CMaterialExpression_Power::StaticClass());
        RegisterGraphNode(CMaterialExpression_Mod::StaticClass());
        RegisterGraphNode(CMaterialExpression_Min::StaticClass());
        RegisterGraphNode(CMaterialExpression_Max::StaticClass());
        RegisterGraphNode(CMaterialExpression_Step::StaticClass());
        RegisterGraphNode(CMaterialExpression_Lerp::StaticClass());

        RegisterGraphNode(CMaterialNodeGetTime::StaticClass());
        RegisterGraphNode(CMaterialExpression_CameraPos::StaticClass());
        RegisterGraphNode(CMaterialExpression_WorldPos::StaticClass());

        RegisterGraphNode(CMaterialExpression_ConstantFloat::StaticClass());
        RegisterGraphNode(CMaterialExpression_ConstantFloat2::StaticClass());
        RegisterGraphNode(CMaterialExpression_ConstantFloat3::StaticClass());
        RegisterGraphNode(CMaterialExpression_ConstantFloat4::StaticClass());

        RegisterGraphNode(CMaterialExpression_TextureSample::StaticClass());

        ValidateGraph();
    }

    void CMaterialNodeGraph::Shutdown()
    {
        CEdNodeGraph::Shutdown();
    }

    void CMaterialNodeGraph::OnDrawGraph()
    {
        
    }

    void CMaterialNodeGraph::CompileGraph(FMaterialCompiler* Compiler)
    {
        LUMINA_PROFILE_SCOPE();
        
        TVector<CEdGraphNode*> SortedNodes;
        TVector<CEdGraphNode*> NodesToEvaluate;
        TSet<CEdGraphNode*> ReachableNodes;
        
        if (Nodes.empty())
        {
            return;
        }

        for (CEdGraphNode* Node : Nodes)
        {
            Node->ClearError();
        }
        
        CEdGraphNode* CyclicNode = TopologicalSort(Nodes, SortedNodes);

        if (CyclicNode != nullptr)
        {
            CyclicNode->SetError("Cyclic");
            FMaterialCompiler::FError Error;
            Error.ErrorName = "Cyclic";
            Error.ErrorDescription = "Cycle detected in material node graph! Graph must be acyclic!";
            Error.ErrorNode = static_cast<CMaterialGraphNode*>(CyclicNode);
            Compiler->AddError(Error);
            return;
        }

        for (int i = 0; i < SortedNodes.size(); ++i)
        {
            CEdGraphNode* Node = SortedNodes[i];
            
            Node->SetDebugExecutionOrder(i);
            if (Node == Nodes[0].Get())
            {
                continue; 
            }

            CMaterialGraphNode* MaterialGraphNode = static_cast<CMaterialGraphNode*>(Node);
            MaterialGraphNode->GenerateExpression(Compiler);
        }

        Compiler->NewLine();
        Compiler->NewLine();

        for (SIZE_T i = 0; i < SortedNodes.size(); ++i)
        {
            CEdGraphNode* Node = SortedNodes[i];
            
            Node->SetDebugExecutionOrder((uint32)i);
            if (Node == Nodes[0].Get())
            {
                continue; 
            }

            CMaterialGraphNode* MaterialGraphNode = static_cast<CMaterialGraphNode*>(Node);
            MaterialGraphNode->GenerateDefinition(Compiler);
        }

        // Start off the compilation process using the MaterialOutput node as the kick-off.
        CMaterialGraphNode* MaterialOutputNode = static_cast<CMaterialGraphNode*>(Nodes[0].Get());
        MaterialOutputNode->GenerateDefinition(Compiler);
        
    }

    void CMaterialNodeGraph::ValidateGraph()
    {
        Connections.clear();
        
        for (CEdGraphNode* Node : Nodes)
        {
            for (CEdNodeGraphPin* InputPin : Node->GetInputPins())
            {
                for (CEdNodeGraphPin* Connection : InputPin->GetConnections())
                {
                    Connections.push_back(InputPin->PinID);
                    Connections.push_back(Connection->PinID);
                }
            }
        }
    }

    void CMaterialNodeGraph::SetMaterial(CMaterial* InMaterial)
    {
        Material = InMaterial;
    }

    CEdGraphNode* CMaterialNodeGraph::CreateNode(CClass* NodeClass)
    {
        CEdGraphNode* NewNode = NewObject<CEdGraphNode>(NodeClass, Material->GetPackage());
        AddNode(NewNode);
        return NewNode;
    }
    
    CEdGraphNode* CMaterialNodeGraph::TopologicalSort(const TVector<TObjectHandle<CEdGraphNode>>& NodesToSort, TVector<CEdGraphNode*>& SortedNodes)
    {
        LUMINA_PROFILE_SCOPE();

        THashMap<CEdGraphNode*, uint32> InDegree;
        TQueue<CEdGraphNode*> ReadyQueue;
        THashSet<CEdGraphNode*> ReachableNodes;
        uint32 ProcessedNodeCount = 0;
    
        // Step 0: Define the root node (output node)
        CEdGraphNode* RootNode = nullptr;
        for (CEdGraphNode* Node : NodesToSort)
        {
            if (Cast<CMaterialOutputNode>(Node))
            {
                RootNode = Node;
                break;
            }
        }
    
        if (!RootNode)
        {
            SortedNodes.clear();
            return nullptr;
        }
    
        // Step 1: Traverse backwards starting from the root node to find all reachable nodes
        TQueue<CEdGraphNode*> ReverseQueue;
        ReverseQueue.push(RootNode);
        ReachableNodes.insert(RootNode);
    
        while (!ReverseQueue.empty())
        {
            CEdGraphNode* Node = ReverseQueue.front();
            ReverseQueue.pop();
    
            // Check input pins (dependencies) to see which nodes can be reached from the current node
            for (CEdNodeGraphPin* InputPin : Node->GetInputPins())
            {
                for (CEdNodeGraphPin* ConnectedPin : InputPin->GetConnections())
                {
                    CEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    if (ReachableNodes.insert(ConnectedNode).second)  // If newly added
                    {
                        ReverseQueue.push(ConnectedNode);
                    }
                }
            }
        }
    
        // Step 2: Initialize in-degree map only for reachable nodes
        for (CEdGraphNode* Node : NodesToSort)
        {
            if (ReachableNodes.find(Node) != ReachableNodes.end())
            {
                InDegree[Node] = 0;
            }
        }
    
        // Step 3: Compute in-degrees only for reachable nodes
        for (CEdGraphNode* Node : NodesToSort)
        {
            if (ReachableNodes.find(Node) == ReachableNodes.end())  // Skip orphaned nodes
                continue;
    
            for (CEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                for (CEdNodeGraphPin* ConnectedPin : OutputPin->GetConnections())
                {
                    CEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    if (ReachableNodes.find(ConnectedNode) != ReachableNodes.end())  // Only count reachable connections
                    {
                        InDegree[ConnectedNode]++;
                    }
                }
            }
        }
    
        // Step 4: Add nodes with zero in-degree to the queue
        for (auto& Pair : InDegree)
        {
            if (Pair.second == 0)
            {
                ReadyQueue.push(Pair.first);
            }
        }
    
        // Step 5: Perform topological sort
        while (!ReadyQueue.empty())
        {
            CEdGraphNode* Node = ReadyQueue.front();
            ReadyQueue.pop();
            SortedNodes.push_back(Node);
            ProcessedNodeCount++;
    
            for (CEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                for (CEdNodeGraphPin* ConnectedPin : OutputPin->GetConnections())
                {
                    CEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    if (ReachableNodes.find(ConnectedNode) == ReachableNodes.end())
                        continue;
    
                    if (--InDegree[ConnectedNode] == 0)
                    {
                        ReadyQueue.push(ConnectedNode);
                    }
                }
            }
        }
    
        // Step 6: Check for cycles (if the number of processed nodes is not equal to the number of reachable nodes, there was a cycle)
        if (ProcessedNodeCount != ReachableNodes.size())
        {
            // Find nodes that still have a nonzero in-degree (they are part of the cycle)
            for (auto& Pair : InDegree)
            {
                if (Pair.second > 0)
                {
                    SortedNodes.clear();
                    return Pair.first;
                }
            }
        }
    
        return nullptr;
    }



}
