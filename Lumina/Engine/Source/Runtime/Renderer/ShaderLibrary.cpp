#include "ShaderLibrary.h"
#include "Containers/String.h"
#include "Platform/Filesystem/FileHelper.h"
#include <shaderc/shaderc.hpp>
#include <filesystem>
#include "Renderer/RHIIncl.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{
    
    
    FShaderLibrary::FShaderLibrary()
    {
    }

    FShaderLibrary::~FShaderLibrary()
    {
        Shutdown();
    }

    void FShaderLibrary::Shutdown()
    {
        Library.clear();
    }

    void FShaderLibrary::LoadShadersInDirectory(const FString& Directory)
    {
        for (const auto& Dir : std::filesystem::recursive_directory_iterator(Directory.c_str()))
        {
            if(Dir.is_directory())
            {
                continue;
            }

            if(Dir.path().extension() == ".vert" || Dir.path().extension() == ".frag")
            {
                LOG_DEBUG("Loading Shader at: {0}", Dir.path().string().c_str());

                
                
            }
        }
    }

    bool FShaderLibrary::Load(const FString& Vertex, const FString& Fragment, FName Tag)
    {
        Assert(Library.find(Tag) == Library.end());

        TRefCountPtr<FShader> NewShader = FShader::Create();
        
        TVector<FShaderStage> Stages =
        {
            { .Stage = EShaderStage::VERTEX,   .RawPath = Vertex },
            { .Stage = EShaderStage::FRAGMENT, .RawPath = Fragment }
        };

        
        for (FShaderStage& Stage : Stages)
        {

            FString ShaderSource;
            if(!FFileHelper::LoadFileIntoString(ShaderSource, Stage.RawPath))
            {
                return false;
            }

            if (FShaderCompiler::Get()->CompileShader(Stage.Binaries, ShaderSource, Tag.c_str()) != EShaderCompileResult::Success)
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

    TRefCountPtr<FShader> FShaderLibrary::GetShader(FName Key)
    {
        if (Get()->Library.find(Key) != Get()->Library.end())
        {
            return Get()->Library[Key];
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
