#pragma once
#include <sstream>

#include "MaterialInput.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class CTexture;
    class FMaterialNodePin;
    class CMaterialGraphNode;
}


namespace Lumina
{
    class FMaterialCompiler
    {
    public:

        FMaterialCompiler()
        {
            ShaderChunks.reserve(4096 * 10);
        }

        struct FError
        {
            FString ErrorName;
            FString ErrorDescription;
            CMaterialGraphNode* ErrorNode = nullptr;
            FMaterialNodePin*   ErrorPin = nullptr;
        };

        struct FScalarParam
        {
            uint16 Index;
            float Value;
        };

        struct FVectorParam
        {
            uint16 Index;
            glm::vec4 Value;
        };
        
        FORCEINLINE bool HasErrors() const { return !Errors.empty(); }
        FORCEINLINE void AddError(const FError& Error) { Errors.push_back(Error); }
        FORCEINLINE const TVector<FError>& GetErrors() const { return Errors; }

        FString BuildTree();

        
        void DefineFloatParameter(const FString& NodeID, const FName& ParamID, float Value);
        void DefineFloat2Parameter(const FString& NodeID, const FName& ParamID, float Value[2]);
        void DefineFloat3Parameter(const FString& NodeID, const FName& ParamID, float Value[3]);
        void DefineFloat4Parameter(const FString& NodeID, const FName& ParamID, float Value[4]);

        void DefineConstantFloat(const FString& ID, float Value);
        void DefineConstantFloat2(const FString& ID, float Value[2]);
        void DefineConstantFloat3(const FString& ID, float Value[3]);
        void DefineConstantFloat4(const FString& ID, float Value[4]);

        void DefineTextureSample(const FString& ID);
        void TextureSample(const FString& ID, CTexture* Texture);

        void NewLine();
        
        void Time(const FString& ID);
        void WorldPos(const FString& ID);
        void CameraPos(const FString& ID);
        
        void Multiply(CMaterialInput* A, CMaterialInput* B);
        void Divide(CMaterialInput* A, CMaterialInput* B);
        void Add(CMaterialInput* A, CMaterialInput* B);
        void Subtract(CMaterialInput* A, CMaterialInput* B);
        void Sin(CMaterialInput* A, CMaterialInput* B);
        void Cos(CMaterialInput* A, CMaterialInput* B);
        void Floor(CMaterialInput* A, CMaterialInput* B);
        void Ceil(CMaterialInput* A, CMaterialInput* B);
        void Power(CMaterialInput* A, CMaterialInput* B);
        void Mod(CMaterialInput* A, CMaterialInput* B);
        void Min(CMaterialInput* A, CMaterialInput* B);
        void Max(CMaterialInput* A, CMaterialInput* B);
        void Step(CMaterialInput* A, CMaterialInput* B);
        void Lerp(CMaterialInput* A, CMaterialInput* B, CMaterialInput* C);

        void GetBoundTextures(TVector<TObjectHandle<CTexture>>& Images);

        void AddRaw(const FString& Raw);

        const THashMap<FName, FScalarParam>& GetScalarParameters() const { return ScalarParameters; }
        const THashMap<FName, FVectorParam>& GetVectorParameters() const { return VectorParameters; }
        
        //-----------------------------------------------------------------------------
    
    private:
        
        FString                             ShaderChunks;
        TVector<FError>                     Errors;

        uint32                              BindingIndex = 1; // 0 is uniform buffer.
        TVector<TObjectHandle<CTexture>>    BoundImages;

        uint32                              NumScalarParams = 0;
        uint32                              NumVectorParams = 0;
        
        THashMap<FName, FScalarParam>       ScalarParameters;
        
        THashMap<FName, FVectorParam>       VectorParameters;

    };
}
