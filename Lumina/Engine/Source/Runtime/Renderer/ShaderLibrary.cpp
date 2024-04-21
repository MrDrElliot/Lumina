#include "ShaderLibrary.h"

#include <fstream>

#include "RHI/Vulkan/VulkanShader.h"
#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    FShaderLibrary* FShaderLibrary::Instance = nullptr;
    
    FShaderLibrary::FShaderLibrary()
    {
    }

    FShaderLibrary::~FShaderLibrary()
    {
        for (auto& [Key, Shader] : Library)
        {
            LE_LOG_WARN("Shader Library: Destroying Shader - {0}", Key);
            Shader->Destroy();
        }
    }

    void FShaderLibrary::Init()
    {
        Instance = new FShaderLibrary;
    }

    void FShaderLibrary::Destroy()
    {
        delete Instance;
    }

    bool FShaderLibrary::Load(std::filesystem::path Vertex, std::filesystem::path Fragment, const std::string& Tag)
    {
        std::vector<FShaderData> Shaders;
        
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
        
        Shaders.push_back(VertData);
        Shaders.push_back(FragData);

        for (auto& Shader : Shaders)
        {
            std::ifstream File(Shader.RawPath, std::ios::ate | std::ios::binary);
            if(!File.is_open())
            {
                LE_LOG_ERROR("Failed to open shader file: {0}", Shader.RawPath.string());
                return false;
            }
            
            size_t Size = (size_t)File.tellg();
            std::vector<uint32_t> Buffer(Size / sizeof(uint32_t));

            File.seekg(0);
            File.read((char*)Buffer.data(), Size);
            File.close();
            Shader.Binaries = Buffer;
        }
        
        std::shared_ptr<FShader> Shader = FShader::Create(Shaders, Tag);
        Mutex.lock();
        Library.emplace(Tag, Shader);
        Mutex.unlock();
        return true;
    }

    bool FShaderLibrary::Unload(std::string Name)
    {
        if (Library.find(Name) == Library.end()) return false;

        Library.find(Name)->second->Destroy();
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

    std::shared_ptr<FShader> FShaderLibrary::GetShader(std::string Key)
    {
        std::shared_ptr<FShader> ReturnShader = Instance->Library.find(Key)->second;
        if(ReturnShader.get())
        {
            return ReturnShader;
        }
        
        LE_LOG_ERROR("Failed to load shader with key: {0}", Key);
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
