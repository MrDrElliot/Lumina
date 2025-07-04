#include "MaterialCompiler.h"

#include "Core/Math/Hash/Hash.h"
#include "Nodes/MaterialNodeExpression.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include "UI/Tools/NodeGraph/EdGraphNode.h"


namespace Lumina
{
    FString FMaterialCompiler::BuildTree()
    {
        FString Result;
        for (const FShaderChunk& Chunk : ShaderChunks)
        {
            Result += Chunk.Data + "\n";
        }

        FString VertexPath = std::filesystem::path(Paths::GetEngineResourceDirectory() / "Shaders/Material.vert").generic_string().c_str();

        FString LoadedString;
        FileHelper::LoadFileIntoString(LoadedString, VertexPath);

        const char* Token = "$VERTEX_REPLACEMENT";
        size_t Pos = LoadedString.find(Token);

        if (Pos != FString::npos)
        {
            LoadedString.replace(Pos, strlen(Token), Result);
        }
        
        return LoadedString;
    }


    void FMaterialCompiler::DefineConstantFloat(const FString& ID, float Value)
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);
        FString ValueString = eastl::to_string(Value);
    
        FShaderChunk Chunk;
        Chunk.Data = "float " + ID + " = " + ValueString + ";";
        
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat2(const FString& ID, float Value[2])
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);

        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", 0.0, 1.0);";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat3(const FString& ID, float Value[3])
    {
        uint32 Hash = Lumina::Hash::GetHash32(ID);

        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", 1.0);";
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
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", " + ValueStringW + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Multiply(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        CMaterialExpression_Multiplication* Node = A->GetOwningNode<CMaterialExpression_Multiplication>();
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
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " * " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Divide(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        CMaterialExpression_Division* Node = A->GetOwningNode<CMaterialExpression_Division>();
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
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " / " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Add(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        CMaterialExpression_Addition* Node = A->GetOwningNode<CMaterialExpression_Addition>();
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
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " + " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Subtract(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        uint32 Hash = Lumina::Hash::GetHash32(OwningNode);

        CMaterialExpression_Subtraction* Node = A->GetOwningNode<CMaterialExpression_Subtraction>();
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
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " - " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::AddRaw(const FString& Raw)
    {
        FShaderChunk Chunk;
        Chunk.Data = Raw;
        ShaderChunks.push_back(Chunk);
    }
}
