#pragma once
#include <sstream>

#include "MaterialInput.h"
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina
{
    class FMaterialNodePin;
    class FMaterialGraphNode;
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
            FMaterialGraphNode* ErrorNode = nullptr;
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

        void Multiply(FMaterialInput* A, FMaterialInput* B);
        void Divide(FMaterialInput* A, FMaterialInput* B);
        void Add(FMaterialInput* A, FMaterialInput* B);
        void Subtract(FMaterialInput* A, FMaterialInput* B);

        void AddRaw(const FString& Raw);
        
        //-----------------------------------------------------------------------------
    
    private:

        FMaterialGraphNode*                 CurrentNode = nullptr;
        FMaterialNodePin*                   CurrentPin =  nullptr;
        TVector<FShaderChunk>               ShaderChunks;
        TVector<FError>                     Errors;
        
    };
}
