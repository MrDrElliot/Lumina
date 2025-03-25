#include "MaterialNodeGraph.h"

#include "MaterialCompiler.h"
#include "Nodes/MaterialNodeExpression.h"
#include "Nodes/MaterialOutputNode.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"


namespace Lumina
{
    FMaterialNodeGraph::FMaterialNodeGraph()
    {
        CreateNode<FMaterialOutputNode>();

        REGISTER_GRAPH_NODE(FMaterialExpression_Addition)
        REGISTER_GRAPH_NODE(FMaterialExpression_Subtraction)
        REGISTER_GRAPH_NODE(FMaterialExpression_Multiplication)
        REGISTER_GRAPH_NODE(FMaterialExpression_ConstantFloat)
        REGISTER_GRAPH_NODE(FMaterialExpression_ConstantFloat2)
        REGISTER_GRAPH_NODE(FMaterialExpression_ConstantFloat3)
        REGISTER_GRAPH_NODE(FMaterialExpression_ConstantFloat4)

    }

    void FMaterialNodeGraph::OnDrawGraph()
    {
        
    }

    void FMaterialNodeGraph::CompileGraph(FMaterialCompiler* Compiler)
    {
        TVector<FEdGraphNode*> SortedNodes;
        TVector<FEdGraphNode*> NodesToEvaluate;
        TSet<FEdGraphNode*> ReachableNodes;
        
        if (Nodes.empty())
        {
            return;
        }

        for (FEdGraphNode* Node : Nodes)
        {
            Node->ClearError();
        }
        
        FEdGraphNode* CyclicNode = TopologicalSort(Nodes, SortedNodes);

        if (CyclicNode != nullptr)
        {
            CyclicNode->SetError("Cyclic");
            FMaterialCompiler::FError Error;
            Error.ErrorName = "Cyclic";
            Error.ErrorDescription = "Cycle detected in material node graph! Graph must be acyclic!";
            Error.ErrorNode = static_cast<FMaterialGraphNode*>(CyclicNode);
            Compiler->AddError(Error);
            return;
        }

        for (int i = 0; i < SortedNodes.size(); ++i)
        {
            FEdGraphNode* Node = SortedNodes[i];
            
            Node->SetDebugExecutionOrder(i);
            if (Node == Nodes[0])
            {
                continue; 
            }

            FMaterialGraphNode* MaterialGraphNode = static_cast<FMaterialGraphNode*>(Node);
            MaterialGraphNode->GenerateDefinition(Compiler);

            LOG_DEBUG("Generating node: {0}", MaterialGraphNode->GetNodeFullName());
        }

        // We then start off the compilation process using the MaterialOutput node as the kick-off.
        FMaterialGraphNode* MaterialOutputNode = static_cast<FMaterialGraphNode*>(Nodes[0]);
        MaterialOutputNode->GenerateDefinition(Compiler);
        
    }

    void FMaterialNodeGraph::ValidateGraph()
    {
        
    }

    FEdGraphNode* FMaterialNodeGraph::TopologicalSort(const TVector<FEdGraphNode*>& NodesToSort, TVector<FEdGraphNode*>& SortedNodes)
    {
        THashMap<FEdGraphNode*, uint32> InDegree;
        TQueue<FEdGraphNode*> ReadyQueue;
        uint32 ProcessedNodeCount = 0;


        // Step 1: Initialize in-degree map
        for (FEdGraphNode* Node : NodesToSort)
        {
            InDegree.insert_or_assign(Node, 0);
        }

        // Step 2: Compute in-degree for each node
        for (FEdGraphNode* Node : NodesToSort)
        {
            for (FEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                for (FEdNodeGraphPin* ConnectedPin : OutputPin->GetConnections())
                {
                    FEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    InDegree[ConnectedNode]++;
                }
            }
        }

        // Step 3: Add nodes with zero in-degree to the queue
        for (FEdGraphNode* Node : NodesToSort)
        {
            if (InDegree[Node] == 0)
            {
                ReadyQueue.push(Node);
            }
        }

        // Step 4: Process the queue
        while (!ReadyQueue.empty())
        {
            FEdGraphNode* Node = ReadyQueue.front();
            ReadyQueue.pop();
            SortedNodes.push_back(Node);
            ProcessedNodeCount++;

            for (FEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                for (FEdNodeGraphPin* ConnectedPin : OutputPin->GetConnections())
                {
                    FEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    InDegree[ConnectedNode]--;

                    if (InDegree[ConnectedNode] == 0)
                    {
                        ReadyQueue.push(ConnectedNode);
                    }
                }
            }
        }

        if (ProcessedNodeCount != NodesToSort.size())
        {
            // Find nodes that still have a nonzero in-degree
            for (const auto& Pair : InDegree)
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
