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

        FString VertexPath = std::filesystem::path(Paths::GetEngineResourceDirectory() / "MaterialShader/Material.vert").generic_string().c_str();

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
        FString ValueString = eastl::to_string(Value);
    
        FShaderChunk Chunk;
        Chunk.Data = "float " + ID + " = " + ValueString + ";";
        
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat2(const FString& ID, float Value[2])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", 0.0, 1.0);";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat3(const FString& ID, float Value[3])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", 1.0);";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::DefineConstantFloat4(const FString& ID, float Value[4])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);
        FString ValueStringW = eastl::to_string(Value[3]);

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", " + ValueStringW + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::WorldPos(const FString& ID)
    {
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(GetModelLocation(), 1.0);";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::CameraPos(const FString& ID)
    {
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(GetCameraPosition(), 1.0);";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Time(const FString& ID)
    {
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = vec4(GetTime(), GetTime(), GetTime(), GetTime());";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Multiply(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
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

    void FMaterialCompiler::Sin(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Sin* Node = A->GetOwningNode<CMaterialExpression_Sin>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString AValue = "vec4(" + ConstAString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = sin(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Cos(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Cosin* Node = A->GetOwningNode<CMaterialExpression_Cosin>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString AValue = "vec4(" + ConstAString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = cos(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Floor(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Floor* Node = A->GetOwningNode<CMaterialExpression_Floor>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString AValue = "vec4(" + ConstAString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = floor(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Ceil(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Ceil* Node = A->GetOwningNode<CMaterialExpression_Ceil>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString AValue = "vec4(" + ConstAString + ")";

        if (A->HasConnection())
        {
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = ceil(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Power(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Power* Node = A->GetOwningNode<CMaterialExpression_Power>();
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
        Chunk.Data = "vec4 " + OwningNode + " = pow(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Mod(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Mod* Node = A->GetOwningNode<CMaterialExpression_Mod>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);

        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (B->HasConnection())
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = mod(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Min(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Min* Node = A->GetOwningNode<CMaterialExpression_Min>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);

        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (B->HasConnection())
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = min(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Max(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Max* Node = A->GetOwningNode<CMaterialExpression_Max>();
        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);

        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (B->HasConnection())
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = max(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Step(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Step* Node = A->GetOwningNode<CMaterialExpression_Step>();
        FString ConstAString = eastl::to_string(Node->ConstA); // edge
        FString ConstBString = eastl::to_string(Node->ConstB); // input

        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";

        if (A->HasConnection())
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (B->HasConnection())
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = step(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Lerp(CMaterialInput* A, CMaterialInput* B, CMaterialInput* C)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Lerp* Node = A->GetOwningNode<CMaterialExpression_Lerp>();

        FString ConstAString = eastl::to_string(Node->ConstA);
        FString ConstBString = eastl::to_string(Node->ConstB);
        FString ConstCString = eastl::to_string(Node->ConstC);

        FString AValue = "vec4(" + ConstAString + ")";
        FString BValue = "vec4(" + ConstBString + ")";
        FString CValue = "vec4(" + ConstCString + ")";

        if (A->HasConnection())
            AValue = A->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (B->HasConnection())
            BValue = B->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        if (C->HasConnection())
            CValue = C->GetConnections()[0]->GetOwningNode()->GetNodeFullName();

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = mix(" + AValue + ", " + BValue + ", " + CValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::AddRaw(const FString& Raw)
    {
        FShaderChunk Chunk;
        Chunk.Data = Raw;
        ShaderChunks.push_back(Chunk);
    }
}
