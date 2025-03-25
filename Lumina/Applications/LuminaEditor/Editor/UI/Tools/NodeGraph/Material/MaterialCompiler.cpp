#include "MaterialCompiler.h"

#include "Core/Math/Hash/Hash.h"
#include "Nodes/MaterialNodeExpression.h"
#include "UI/Tools/NodeGraph/EdGraphNode.h"


namespace Lumina
{
    FString FMaterialCompiler::BuildTree()
    {
        FString Result;
        for (const FShaderChunk& Chunk : ShaderChunks)
        {
            Result += Chunk.Data + "; \n";
        }
        return Result;
    }


    void FMaterialCompiler::DefineConstantFloat(const FString& ID, float Value)
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);
        FString ValueString = eastl::to_string(Value);
    
        FShaderChunk Chunk;
        Chunk.Data = "float " + ID + " = " + ValueString;
        
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat2(const FString& ID, float Value[2])
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);

        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", 0.0, 1.0)";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat3(const FString& ID, float Value[3])
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);

        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", 1.0)";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat4(const FString& ID, float Value[4])
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);

        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);
        FString ValueStringW = eastl::to_string(Value[3]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", " + ValueStringW + ")";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Multiply(FMaterialInput* A, FMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        FMaterialExpression_Multiplication* Node = A->GetOwningNode<FMaterialExpression_Multiplication>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);
    
        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        if (B->HasConnection())
        {
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }
    
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " * " + BValue;
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Divide(FMaterialInput* A, FMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        FMaterialExpression_Division* Node = A->GetOwningNode<FMaterialExpression_Division>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);
    
        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        if (B->HasConnection())
        {
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }
    
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " / " + BValue;
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Add(FMaterialInput* A, FMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        FMaterialExpression_Addition* Node = A->GetOwningNode<FMaterialExpression_Addition>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);
    
        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        if (B->HasConnection())
        {
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }
    
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " + " + BValue;
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Subtract(FMaterialInput* A, FMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        FMaterialExpression_Subtraction* Node = A->GetOwningNode<FMaterialExpression_Subtraction>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);
    
        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        if (B->HasConnection())
        {
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }
    
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " - " + BValue;
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::AddRaw(const FString& Raw)
    {
        FShaderChunk Chunk;
        Chunk.Data = Raw;
        ShaderChunks.push_back(Chunk);
    }
}
