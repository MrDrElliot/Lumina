﻿#pragma once
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
    
    using ShaderBinaries = TVector<uint32>;
    
    class IShaderCompiler
    {
    public:
        using CompletedFunc = TFunction<void(const TVector<uint32>& Binaries)>;

        virtual ~IShaderCompiler() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual bool CompilerShaderRaw(const FString& ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) = 0;
        virtual bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) = 0;

        virtual bool HasPendingRequests() const = 0;
        
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

        bool CompilerShaderRaw(const FString& ShaderString, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) override;
        bool CompileShader(const FString& ShaderPath, const FShaderCompileOptions& CompileOptions, CompletedFunc OnCompleted) override;

        void AddRequest(const FRequest& Request);
        void PopRequest();

        bool HasPendingRequests() const override { return !PendingRequest.empty(); }
        
        FMutex           RequestMutex;
        TQueue<FRequest> PendingRequest;
    };
}
