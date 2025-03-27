#include "MaterialNodeGraph.h"

#include "MaterialCompiler.h"
#include "Nodes/MaterialNodeExpression.h"
#include "Nodes/MaterialOutputNode.h"
#include "UI/Tools/NodeGraph/EdNodeGraphPin.h"


namespace Lumina
{
    IMPLEMENT_CLASS(CMaterialNodeGraph)

    CMaterialNodeGraph::CMaterialNodeGraph()
    {
        CreateNode<CMaterialOutputNode>();

        REGISTER_GRAPH_NODE(CMaterialExpression_Addition)
        REGISTER_GRAPH_NODE(CMaterialExpression_Subtraction)
        REGISTER_GRAPH_NODE(CMaterialExpression_Multiplication)
        REGISTER_GRAPH_NODE(CMaterialExpression_ConstantFloat)
        REGISTER_GRAPH_NODE(CMaterialExpression_ConstantFloat2)
        REGISTER_GRAPH_NODE(CMaterialExpression_ConstantFloat3)
        REGISTER_GRAPH_NODE(CMaterialExpression_ConstantFloat4)

    }

    void CMaterialNodeGraph::OnDrawGraph()
    {
        
    }

    void CMaterialNodeGraph::CompileGraph(FMaterialCompiler* Compiler)
    {
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
            if (Node == Nodes[0])
            {
                continue; 
            }

            CMaterialGraphNode* MaterialGraphNode = static_cast<CMaterialGraphNode*>(Node);
            MaterialGraphNode->GenerateDefinition(Compiler);

            LOG_DEBUG("Generating node: {0}", MaterialGraphNode->GetNodeFullName());
        }

        // We then start off the compilation process using the MaterialOutput node as the kick-off.
        CMaterialGraphNode* MaterialOutputNode = static_cast<CMaterialGraphNode*>(Nodes[0]);
        MaterialOutputNode->GenerateDefinition(Compiler);
        
    }

    void CMaterialNodeGraph::ValidateGraph()
    {
        
    }

    CEdGraphNode* CMaterialNodeGraph::TopologicalSort(const TVector<CEdGraphNode*>& NodesToSort, TVector<CEdGraphNode*>& SortedNodes)
    {
        THashMap<CEdGraphNode*, uint32> InDegree;
        TQueue<CEdGraphNode*> ReadyQueue;
        uint32 ProcessedNodeCount = 0;


        // Step 1: Initialize in-degree map
        for (CEdGraphNode* Node : NodesToSort)
        {
            InDegree.insert_or_assign(Node, 0);
        }

        // Step 2: Compute in-degree for each node
        for (CEdGraphNode* Node : NodesToSort)
        {
            for (CEdNodeGraphPin* OutputPin : Node->GetOutputPins())
            {
                for (CEdNodeGraphPin* ConnectedPin : OutputPin->GetConnections())
                {
                    CEdGraphNode* ConnectedNode = ConnectedPin->GetOwningNode();
                    InDegree[ConnectedNode]++;
                }
            }
        }

        // Step 3: Add nodes with zero in-degree to the queue
        for (CEdGraphNode* Node : NodesToSort)
        {
            if (InDegree[Node] == 0)
            {
                ReadyQueue.push(Node);
            }
        }

        // Step 4: Process the queue
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
