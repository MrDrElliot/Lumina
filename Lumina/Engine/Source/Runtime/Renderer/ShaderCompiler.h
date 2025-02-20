#pragma once
#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"


namespace Lumina
{
    struct FShaderCompileRequest
    {
        FString ShaderData;
    };

    enum EShaderCompileResult
    {
        Success,
        Failed,
        InProgress,
    };

    
    class FShaderCompiler : public TSingleton<FShaderCompiler>
    {
    public:

        EShaderCompileResult CompileShader(TVector<uint32>& OutBinaries, const FStringView& ShaderData, const FStringView& FileName);


    protected:

        

    private:

        TQueue<FShaderCompileRequest> CompileRequests;
    
    };
}