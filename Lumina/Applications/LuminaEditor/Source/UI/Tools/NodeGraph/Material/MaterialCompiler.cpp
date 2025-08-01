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
        FString FragmentPath = Paths::GetEngineResourceDirectory() + "/MaterialShader/Material.frag";

        FString LoadedString;
        FileHelper::LoadFileIntoString(LoadedString, FragmentPath);

        const char* Token = "$MATERIAL_INPUTS";
        size_t Pos = LoadedString.find(Token);

        if (Pos != FString::npos)
        {
            LoadedString.replace(Pos, strlen(Token), ShaderChunks);
        }
        
        return LoadedString;
    }

    void FMaterialCompiler::DefineFloatParameter(const FString& NodeID, const FName& ParamID, float Value)
    {
        if (ScalarParameters.find(ParamID) == ScalarParameters.end())
        {
            ScalarParameters[ParamID].Index = NumScalarParams++;
            ScalarParameters[ParamID].Value = Value;
        }

        FString IndexString = eastl::to_string(ScalarParameters[ParamID].Index);
        ShaderChunks.append("float " + NodeID + " = GetMaterialScalar(" + IndexString + ");\n");
    }

    void FMaterialCompiler::DefineFloat2Parameter(const FString& NodeID, const FName& ParamID, float Value[2])
    {
        if (VectorParameters.find(ParamID) == VectorParameters.end())
        {
            VectorParameters[ParamID].Index = NumVectorParams++;
            VectorParameters[ParamID].Value = glm::vec4(Value[0], Value[1], 1.0f, 1.0f);
        }

        FString IndexString = eastl::to_string(VectorParameters[ParamID].Index);
        
        ShaderChunks.append("vec4 " + NodeID + " = GetMaterialVec4(" + IndexString + ");\n");
    }

    void FMaterialCompiler::DefineFloat3Parameter(const FString& NodeID, const FName& ParamID, float Value[3])
    {
        if (VectorParameters.find(ParamID) == VectorParameters.end())
        {
            VectorParameters[ParamID].Index = NumVectorParams++;
            VectorParameters[ParamID].Value = glm::vec4(Value[0], Value[1], Value[2], 1.0f);
        }
        
        FString IndexString = eastl::to_string(VectorParameters[ParamID].Index);
        ShaderChunks.append("vec4 " + NodeID + " = GetMaterialVec4(" + IndexString + ");\n");
    }

    void FMaterialCompiler::DefineFloat4Parameter(const FString& NodeID, const FName& ParamID, float Value[4])
    {
        if (VectorParameters.find(ParamID) == VectorParameters.end())
        {
            VectorParameters[ParamID].Index = NumVectorParams++;
            VectorParameters[ParamID].Value = glm::vec4(Value[0], Value[1], Value[2], Value[3]);
        }
        
        FString IndexString = eastl::to_string(VectorParameters[ParamID].Index);
        ShaderChunks.append("vec4 " + NodeID + " = GetMaterialVec4(" + IndexString + ");\n");
    }


    void FMaterialCompiler::DefineConstantFloat(const FString& ID, float Value)
    {
        FString ValueString = eastl::to_string(Value);
    
        ShaderChunks.append("float " + ID + " = " + ValueString + ";\n");
    }

    void FMaterialCompiler::DefineConstantFloat2(const FString& ID, float Value[2])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);

        ShaderChunks.append("vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", 0.0, 1.0);\n");
    }

    void FMaterialCompiler::DefineConstantFloat3(const FString& ID, float Value[3])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);

        ShaderChunks.append("vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", 1.0);\n");
    }

    void FMaterialCompiler::DefineConstantFloat4(const FString& ID, float Value[4])
    {
        FString ValueStringX = eastl::to_string(Value[0]);
        FString ValueStringY = eastl::to_string(Value[1]);
        FString ValueStringZ = eastl::to_string(Value[2]);
        FString ValueStringW = eastl::to_string(Value[3]);

        ShaderChunks.append("vec4 " + ID + " = vec4(" + ValueStringX + ", " + ValueStringY + ", " + ValueStringZ + ", " + ValueStringW + ");\n");
    }

    void FMaterialCompiler::DefineTextureSample(const FString& ID)
    {
        ShaderChunks.append("layout(set = 1, binding = " + eastl::to_string(BindingIndex) + ") uniform sampler2D " + ID + "_sample;\n");
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
        
        ShaderChunks.append("vec4 " + ID + " = texture(" + ID + "_sample" + ", inUV.xy);\n");
        BoundImages.push_back(Texture);
    }

    void FMaterialCompiler::NewLine()
    {
        ShaderChunks.append("\n");
    }

    void FMaterialCompiler::WorldPos(const FString& ID)
    {
        ShaderChunks.append("vec4 " + ID + " = vec4(inModelMatrix[3].xyz, 1.0);\n");
    }

    void FMaterialCompiler::CameraPos(const FString& ID)
    {
        ShaderChunks.append("vec4 " + ID + " = vec4(GetCameraPosition(), 1.0);\n");
    }

    void FMaterialCompiler::Time(const FString& ID)
    {
        ShaderChunks.append("vec4 " + ID + " = vec4(GetTime());\n");
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

        ShaderChunks.append(ResultType + " " + OwningNode + " = " + AValue + " * " + BValue + ";\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = " + AValue + " / " + BValue + ";\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = " + AValue + " + " + BValue + ";\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = " + AValue + " - " + BValue + ";\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = sin(" + AValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = cos(" + AValue + ");");
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

        ShaderChunks.append("vec4 " + OwningNode + " = floor(" + AValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = ceil(" + AValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = pow(" + AValue + ", " + BValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = mod(" + AValue + ", " + BValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = min(" + AValue + ", " + BValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = max(" + AValue + ", " + BValue + ");\n");
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

        ShaderChunks.append("vec4 " + OwningNode + " = step(" + AValue + ", " + BValue + ");\n");
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
            FString ConstCString = eastl::to_string(Node->Alpha);
            CValue = "vec4(" + ConstCString + ")";
        }

        ShaderChunks.append("vec4 " + OwningNode + " = mix(" + AValue + ", " + BValue + ", " + CValue + ");\n");
    }

    void FMaterialCompiler::GetBoundTextures(TVector<TObjectHandle<CTexture>>& Images)
    {
        Images = BoundImages;
    }

    void FMaterialCompiler::AddRaw(const FString& Raw)
    {
        ShaderChunks.append(Raw);
    }
}
