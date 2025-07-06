#pragma once
#include <sstream>

#include "MaterialInput.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class CTexture;
}

namespace Lumina
{
    class FMaterialNodePin;
    class CMaterialGraphNode;
}

namespace Lumina
{

    class FShaderChunk
    {
    public:
        
        FString Data;
    };
    
    class FMaterialCompiler
    {
    public:

        struct FError
        {
            FString ErrorName;
            FString ErrorDescription;
            CMaterialGraphNode* ErrorNode = nullptr;
            FMaterialNodePin*   ErrorPin = nullptr;
        };

        
        FORCEINLINE bool HasErrors() const { return !Errors.empty(); }
        FORCEINLINE void AddError(const FError& Error) { Errors.push_back(Error); }
        FORCEINLINE const TVector<FError>& GetErrors() const { return Errors; }

        FString BuildTree();
        
        
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

        void GetBoundImages(TVector<FRHIImageRef>& Images);

        void AddRaw(const FString& Raw);
        
        //-----------------------------------------------------------------------------
    
    private:

        CMaterialGraphNode*                 CurrentNode = nullptr;
        FMaterialNodePin*                   CurrentPin =  nullptr;
        TVector<FShaderChunk>               ShaderChunks;
        TVector<FError>                     Errors;

        uint32                              BindingIndex = 0;
        TVector<FRHIImageRef>               BoundImages;
    };
}
