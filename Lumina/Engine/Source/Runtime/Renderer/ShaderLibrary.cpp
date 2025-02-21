#include "ShaderLibrary.h"
#include "Containers/String.h"
#include "Platform/Filesystem/FileHelper.h"
#include <shaderc/shaderc.hpp>
#include <filesystem>

#include "TaskScheduler.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Renderer/RHIIncl.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    
    FShaderLibrary::FShaderLibrary()
    {
    }
    
    void FShaderLibrary::Shutdown()
    {
        Library.clear();
    }

    void FShaderLibrary::LoadShadersInDirectory(const FString& Directory)
    {
        for (const auto& Dir : std::filesystem::directory_iterator(Directory.c_str()))
        {
            if(Dir.is_directory())
            {
                continue;
            }

            if(Dir.path().extension() == ".glsl")
            {
                FShaderLoadRequest* Request = FMemory::New<FShaderLoadRequest>(Dir.path().string().c_str(), Dir.path().filename().string().c_str());
                FTaskSystem::Get()->ScheduleTask(Request);
            }
        }
    }

    bool FShaderLibrary::Load(const FString& Shader, FName Tag)
    {
        Assert(Library.find(Tag) == Library.end());

        LOG_DEBUG("Compiling Shader: {0}", Tag.c_str());
        
        TRefCountPtr<FShader> NewShader = FShader::Create();
        
        TVector<FShaderStage> Stages =
        {
            { .Stage = EShaderStage::VERTEX,   .RawPath = Shader },
            { .Stage = EShaderStage::FRAGMENT, .RawPath = Shader }
        };


        FString ShaderSource;
        if(!FFileHelper::LoadFileIntoString(ShaderSource, Shader))
        {
            LOG_ERROR("Failed to open shader file! {0}", Tag.c_str());
            return false;
        }
        
        for (FShaderStage& Stage : Stages)
        {
            if (FShaderCompiler::Get()->CompileShader(Stage, ShaderSource) != EShaderCompileResult::Success)
            {
                LOG_ERROR("Failed to compile shader! {0}", Tag.c_str());
                return false;
            }

            SpvReflectShaderModule ReflectModule;
            if (spvReflectCreateShaderModule((size_t)Stage.Binaries.size() * 4, Stage.Binaries.data(), &ReflectModule) != SPV_REFLECT_RESULT_SUCCESS)
            {
                LOG_ERROR("Failed to reflect shader {0}", Tag.c_str());
                return false;
            }

            
            NewShader->CreateStage(Stage);
            NewShader->GenerateShaderStageReflectionData(Stage, &ReflectModule);

            
            spvReflectDestroyShaderModule(&ReflectModule);
        }

        NewShader->GeneratePlatformShaderStageReflectionData(NewShader->GetShaderReflectionData());
        
        Mutex.lock();
        Library.emplace(Tag, NewShader);
        Mutex.unlock();

        return true;
    }


    bool FShaderLibrary::Unload(FName Name)
    {
        if (Library.find(Name) == Library.end())
        {
            return false;
        }
        
        Library.erase(Name);

        return true;
    }

    bool FShaderLibrary::Reload(FName Key)
    {
        return false;
    }

    bool FShaderLibrary::Has(FName Key)
    {
        std::scoped_lock<std::shared_mutex> lock(Mutex);
        return Library.find(Key) != Library.end();
    }

    FRHIShader FShaderLibrary::GetShader(FName Key)
    {
        if (Library.find(Key) != Library.end())
        {
            return Library[Key];
        }
        
        LOG_ERROR("Failed to load shader with key: {0}", Key);
        return nullptr;
    }

    EShaderStage FShaderLibrary::EvaluateStage(std::filesystem::path File) const
    {
        if (File.extension().string() == ".vert")
            return EShaderStage::VERTEX;
        if (File.extension().string() == ".frag")
            return EShaderStage::FRAGMENT;
        if (File.extension().string() == ".comp")
            return EShaderStage::COMPUTE;

        return EShaderStage::UNKNOWN;
    }
}
