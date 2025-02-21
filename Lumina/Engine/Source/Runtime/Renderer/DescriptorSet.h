#pragma once

#include <glm/glm.hpp>
#include "Containers/Name.h"
#include "RenderResource.h"
#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Containers/Array.h"
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    enum class EShaderStage : uint8;

    enum class EDescriptorBindingRate
    {
        SCENE = 0,
        PASS = 1,
        DRAW_CALL = 2
    };
    
    enum class EDescriptorFlags : uint32
    {
        PARTIALLY_BOUND = 1 << 0,
    };
    
    struct FDescriptorSetSpecification
    {
        TVector<FDescriptorBinding> Bindings;
    };
    
    class FDescriptorSet : public IRenderResource
    {
    public:
        virtual ~FDescriptorSet() = default;

        static FRHIDescriptorSet Create(const FDescriptorSetSpecification& InSpec);
        
        virtual void Write(uint16 Binding, uint16 ArrayElement, FRHIBuffer Buffer, uint64 Size, uint64 Offset) = 0;
        virtual void Write(uint16 Binding, uint16 ArrayElement, FRHIImage Image) = 0;
        virtual void Write(uint16 Binding, uint16 ArrayElement, TVector<FRHIImage> Images) = 0;

        FORCEINLINE bool ContainsBinding(const FName& BindingKey) { return DescriptorBindings.find(BindingKey) != DescriptorBindings.end(); }
        FORCEINLINE const FDescriptorBinding& GetDescriptorBinding(const FName& BindingKey) { Assert(ContainsBinding(BindingKey)); return DescriptorBindings[BindingKey]; }

        virtual void* GetPlatformDescriptorSet() const = 0;
        virtual void* GetPlatformDescriptorSetLayout() const = 0;

    protected:
        
        THashMap<FName, FDescriptorBinding> DescriptorBindings;
    };
}
