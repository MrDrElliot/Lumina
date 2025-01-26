#include "ShaderLibrary.h"

#include <fstream>
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
        for (auto& [Key, Shader] : Library)
        {
            LOG_TRACE("Shader Library: Destroying Shader - {0}", Key);
            Shader->Release();
        }

        Library.clear();
    }

    bool FShaderLibrary::Load(std::filesystem::path Vertex, std::filesystem::path Fragment, const LString& Tag)
    {
        TArray<FShaderData> Shaders;
        
        FShaderData VertData
        {
            .Stage = EShaderStage::VERTEX,
            .RawPath = Vertex
        };

        FShaderData FragData
        {
            .Stage = EShaderStage::FRAGMENT,
            .RawPath = Fragment,
        };
        
        Shaders.PushBack(VertData);
        Shaders.PushBack(FragData);

        for (FShaderData& Shader : Shaders)
        {
            std::ifstream File(Shader.RawPath, std::ios::ate | std::ios::binary);
            if(!File.is_open())
            {
                LOG_ERROR("Failed to open shader file: {0}", Shader.RawPath.string());
                return false;
            }
            
            size_t Size = (size_t)File.tellg();
            TArray<uint32> Buffer(Size / sizeof(uint32));

            File.seekg(0);
            File.read((char*)Buffer.data(), (uint64)Size);
            File.close();
            Shader.Binaries = Buffer;
        }
        
        Mutex.lock();
        Library.emplace(Tag, FShader::Create(Shaders, Tag));
        Mutex.unlock();
        return true;
    }

    bool FShaderLibrary::Unload(std::string Name)
    {
        if (Library.find(Name) == Library.end())
        {
            return false;
        }
        
        Library.erase(Name);

        return true;
    }

    bool FShaderLibrary::Reload(std::filesystem::path Name)
    {
        return false;
    }

    bool FShaderLibrary::Has(std::string Key)
    {
        std::scoped_lock<std::shared_mutex> lock(Mutex);
        return Library.find(Key) != Library.end();
    }

    TRefPtr<FShader> FShaderLibrary::GetShader(const std::string& Key)
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
