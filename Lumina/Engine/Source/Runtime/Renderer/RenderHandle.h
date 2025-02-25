#pragma once
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    class FRenderHandle
    {
    public:

        FRenderHandle() = default;
        
        FORCEINLINE uint32 GetHandle() const { return Handle; }
        FORCEINLINE uint32 GetGeneration() const { return Generation; }
        
    private:


        uint32 Handle = 0;
        uint32 Generation = 0;
    
    };

    enum class ERenderHandleType : uint8
    {
        Buffer,
        Image,
        Shader,
        CommandBuffer,
    };


    template <ERenderHandleType Type>
    class TRenderResourceHandle : public FRenderHandle
    {
        static constexpr ERenderHandleType HandleType = Type;
        static constexpr ERenderHandleType GetHandleType() { return HandleType; }
    };
    
}
