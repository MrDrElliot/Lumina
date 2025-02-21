#pragma once
#include <fstream>
#include <shaderc/shaderc.hpp>

#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"


namespace Lumina
{
    struct FShaderStage;
}

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

        EShaderCompileResult CompileShader(FShaderStage& InStage, const FStringView& ShaderData);


    protected:

    
    private:

        TQueue<FShaderCompileRequest> CompileRequests;
        
    };

    
}
