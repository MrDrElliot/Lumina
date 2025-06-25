#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Lumina
{
    struct FShaderCompileOptions
    {
        TVector<FString> MacroDefinitions;
    };
    
    class IShaderCompiler
    {
    public:

        virtual ~IShaderCompiler() = default;

        virtual bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, TVector<uint32>& OutBinaries) = 0;

        
    };


    class FSpirVReflector
    {
    public:

        
    };
    
    class FSpirVShaderCompiler : public IShaderCompiler
    {
    public:

        bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, TVector<uint32>& OutBinaries) override;

        
        
    };
}
