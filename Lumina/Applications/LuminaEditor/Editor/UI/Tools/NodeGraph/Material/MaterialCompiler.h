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

        FString GetResult() const { return StringStream.str().c_str(); }
        
        FORCEINLINE bool HasErrors() const { return !Errors.empty(); }
        FORCEINLINE void AddError(const FError& Error) { Errors.push_back(Error); }
        FORCEINLINE const TVector<FError>& GetErrors() const { return Errors; }


        uint32 Add(uint32 A, uint32 B);
        uint32 Subtract(uint32 A, uint32 B);
        uint32 Multiply(uint32 A, uint32 B);
        uint32 Divide(uint32 A, uint32 B);

        //-----------------------------------------------------------------------------
        
        FMaterialCompiler* operator << (const FString& str)
        {
            StringStream << str.c_str();
            return this;
        }
        
        FMaterialCompiler* operator << (const std::string& str)
        {
            StringStream << str;
            return this;
        }

        FMaterialCompiler* operator << (const char* str)
        {
            StringStream << str;
            return this;
        }

    private:

        FMaterialGraphNode*         CurrentNode = nullptr;
        FMaterialNodePin*           CurrentPin =  nullptr;
        std::stringstream           StringStream;
        TVector<FError>             Errors;
        
    };
}
