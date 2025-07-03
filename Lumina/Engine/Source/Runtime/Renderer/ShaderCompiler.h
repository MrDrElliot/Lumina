#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Functional/Function.h"
#include "Core/Threading/Thread.h"
#include "EASTL/internal/atomic/atomic.h"

namespace Lumina
{
    struct FShaderCompileOptions
    {
        TVector<FString> MacroDefinitions;
    };
    
    class IShaderCompiler
    {
    public:
        
        using CompletedFunc = TFunction<void(const TVector<uint32>& Binaries)>;

        virtual ~IShaderCompiler() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        
        virtual bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) = 0;
        
        
    };


    class FSpirVReflector
    {
    public:

        
    };
    
    class FSpirVShaderCompiler : public IShaderCompiler
    {
    public:
        struct FRequest
        {
            FString Path;
            FShaderCompileOptions CompileOptions;
            CompletedFunc OnCompleted;
        };
        
        void Initialize() override;
        void Shutdown() override;
        
        bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) override;

        void OnCompileThread();
        
        TQueue<FRequest>            CompileRequests;
        std::thread		            CompileThread;
        eastl::atomic<bool>	        bCompileThreadRunning = true;

        FMutex                      RunMutex;
        std::mutex			        FlushMutex;
        std::condition_variable     CompileCV;

    };
}
