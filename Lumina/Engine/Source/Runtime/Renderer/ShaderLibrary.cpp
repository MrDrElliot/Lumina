#include "RenderResource.h"

namespace Lumina
{
    void FShaderLibrary::AddShader(FRHIShader* Shader)
    {
        FScopeLock Lock(Mutex);
        Shaders.insert_or_assign(Shader->GetKey(), Shader);
    }

    void FShaderLibrary::RemoveShader(FName Key)
    {
        FScopeLock Lock(Mutex);
        auto It = Shaders.find(Key);
        Assert(It != Shaders.end())
        
        Shaders.erase(It);
    }

    FRHIShaderRef FShaderLibrary::GetShader(const FName& Key)
    {
        auto It = Shaders.find(Key);
        if (It != Shaders.end())
        {
            return It->second;
        }
        
        return nullptr;
    }
}
