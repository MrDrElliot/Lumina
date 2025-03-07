#include "RenderResource.h"

namespace Lumina
{
    void FShaderLibrary::AddShader(FRHIShader* Shader)
    {
        Shaders.insert_or_assign(Shader->GetKey(), Shader);
    }

    void FShaderLibrary::RemoveShader(FName Key)
    {
        auto It = Shaders.find(Key);
        Assert(It != Shaders.end());
        
        Shaders.erase(It);
    }

    FRHIShaderRef FShaderLibrary::GetShader(FName Key)
    {
        auto It = Shaders.find(Key);
        if (It != Shaders.end())
        {
            return It->second;
        }

        
        return nullptr;
    }
}
