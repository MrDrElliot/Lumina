#pragma once

#include <shaderc/shaderc.hpp>

#include "Shader.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/Function.h"
#include "Core/Threading/Thread.h"

namespace Lumina
{
    struct FShaderCompileOptions
    {
        TVector<FString> MacroDefinitions;
    };
    
    
    class IShaderCompiler
    {
    public:
        using CompletedFunc = TFunction<void(FShaderHeader)>;

        virtual ~IShaderCompiler() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual bool CompilerShaderRaw(FStringView ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) = 0;
        virtual bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) = 0;

        virtual bool HasPendingRequests() const = 0;
        void Flush() const
        {
            while (HasPendingRequests()) { }
        }
        
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

        FSpirVShaderCompiler();
        void Initialize() override;
        void Shutdown() override;

        bool CompilerShaderRaw(FStringView ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) override;
        bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) override;
        void ReflectSpirv(TSpan<uint32> SpirV, FShaderReflection& Reflection);

        void PushRequest(const FRequest& Request);
        void PopRequest();

        bool HasPendingRequests() const override { return !PendingRequest.empty(); }

        shaderc::Compiler           Compiler;
        FMutex                      RequestMutex;
        TQueue<FRequest>            PendingRequest;
    };
}
