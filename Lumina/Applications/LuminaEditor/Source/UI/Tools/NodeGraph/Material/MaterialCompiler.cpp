#include "MaterialCompiler.h"

#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Object/Cast.h"
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

        FString FragmentPath = std::filesystem::path(Paths::GetEngineResourceDirectory() / "MaterialShader/Material.frag").generic_string().c_str();

        FString LoadedString;
        FileHelper::LoadFileIntoString(LoadedString, FragmentPath);

        const char* Token = "$MATERIAL_INPUTS";
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

    void FMaterialCompiler::DefineTextureSample(const FString& ID)
    {
        FShaderChunk Chunk;
        Chunk.Data = "layout(set = 1, binding = " + eastl::to_string(BindingIndex) + ") uniform sampler2D " + ID + "_sample;";
        ShaderChunks.push_back(Chunk);
        BindingIndex++;
    }

    void FMaterialCompiler::TextureSample(const FString& ID, CTexture* Texture)
    {
        if (Texture == nullptr)
        {
            return;
        }
        if (Texture->RHIImage == nullptr)
        {
            return;
        }
        
        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + ID + " = texture(" + ID + "_sample" + ", inUV.xy);";
        ShaderChunks.push_back(Chunk);
        BoundImages.push_back(Texture->RHIImage);
    }

    void FMaterialCompiler::NewLine()
    {
        FShaderChunk Chunk;
        Chunk.Data = "\n";
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
        Chunk.Data = "vec4 " + ID + " = vec4(GetTime());";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Multiply(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Multiplication* Node = A->GetOwningNode<CMaterialExpression_Multiplication>();

        FString AValue, BValue;

        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB);
            BValue = "vec4(" + ConstBString + ")";
        }

        FString ResultType = "vec4";

        FShaderChunk Chunk;
        Chunk.Data = ResultType + " " + OwningNode + " = " + AValue + " * " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Divide(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Division* Node = A->GetOwningNode<CMaterialExpression_Division>();

        FString AValue, BValue;

        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            AValue = "vec4(" + eastl::to_string(Node->ConstA) + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            BValue = "vec4(" + eastl::to_string(Node->ConstB) + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " / " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }


    void FMaterialCompiler::Add(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Addition* Node = A->GetOwningNode<CMaterialExpression_Addition>();

        FString AValue, BValue;

        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            AValue = "vec4(" + eastl::to_string(Node->ConstA) + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            BValue = "vec4(" + eastl::to_string(Node->ConstB) + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " + " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }



    void FMaterialCompiler::Subtract(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Subtraction* Node = A->GetOwningNode<CMaterialExpression_Subtraction>();

        FString AValue, BValue;

        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            AValue = "vec4(" + eastl::to_string(Node->ConstA) + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            BValue = "vec4(" + eastl::to_string(Node->ConstB) + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = " + AValue + " - " + BValue + ";";
        ShaderChunks.push_back(Chunk);
    }
    
    void FMaterialCompiler::Sin(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Sin* Node = A->GetOwningNode<CMaterialExpression_Sin>();

        FString AValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = sin(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Cos(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Cosin* Node = A->GetOwningNode<CMaterialExpression_Cosin>();

        FString AValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = cos(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Floor(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Floor* Node = A->GetOwningNode<CMaterialExpression_Floor>();

        FString AValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = floor(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Ceil(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Ceil* Node = A->GetOwningNode<CMaterialExpression_Ceil>();

        FString AValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = ceil(" + AValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Power(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Power* Node = A->GetOwningNode<CMaterialExpression_Power>();

        FString AValue, BValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB);
            BValue = "vec4(" + ConstBString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = pow(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Mod(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Mod* Node = A->GetOwningNode<CMaterialExpression_Mod>();

        FString AValue, BValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB);
            BValue = "vec4(" + ConstBString + ")";
        }

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

        FString AValue, BValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB);
            BValue = "vec4(" + ConstBString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = max(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Step(CMaterialInput* A, CMaterialInput* B)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Step* Node = A->GetOwningNode<CMaterialExpression_Step>();

        FString AValue, BValue;
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA); // edge
            AValue = "vec4(" + ConstAString + ")";
        }

        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB); // input
            BValue = "vec4(" + ConstBString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = step(" + AValue + ", " + BValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::Lerp(CMaterialInput* A, CMaterialInput* B, CMaterialInput* C)
    {
        FString OwningNode = A->GetOwningNode()->GetNodeFullName();
        CMaterialExpression_Lerp* Node = A->GetOwningNode<CMaterialExpression_Lerp>();

        FString AValue, BValue, CValue;

        // Handle A input
        if (A->HasConnection())
        {
            CMaterialOutput* AConn = Cast<CMaterialOutput>(A->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(AConn->GetComponentMask());
            AValue = "vec4(" + AConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstAString = eastl::to_string(Node->ConstA);
            AValue = "vec4(" + ConstAString + ")";
        }

        // Handle B input
        if (B->HasConnection())
        {
            CMaterialOutput* BConn = Cast<CMaterialOutput>(B->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(BConn->GetComponentMask());
            BValue = "vec4(" + BConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstBString = eastl::to_string(Node->ConstB);
            BValue = "vec4(" + ConstBString + ")";
        }

        // Handle C input
        if (C->HasConnection())
        {
            CMaterialOutput* CConn = Cast<CMaterialOutput>(C->GetConnections()[0]);
            FString Swizzle = GetSwizzleForMask(CConn->GetComponentMask());
            CValue = "vec4(" + CConn->GetOwningNode()->GetNodeFullName() + Swizzle + ")";
        }
        else
        {
            FString ConstCString = eastl::to_string(Node->ConstC);
            CValue = "vec4(" + ConstCString + ")";
        }

        FShaderChunk Chunk;
        Chunk.Data = "vec4 " + OwningNode + " = mix(" + AValue + ", " + BValue + ", " + CValue + ");";
        ShaderChunks.push_back(Chunk);
    }

    void FMaterialCompiler::GetBoundImages(TVector<FRHIImageRef>& Images)
    {
        Images = BoundImages;
    }

    void FMaterialCompiler::AddRaw(const FString& Raw)
    {
        FShaderChunk Chunk;
        Chunk.Data = Raw;
        ShaderChunks.push_back(Chunk);
    }
}
