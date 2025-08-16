#pragma once
#include "RenderContext.h"
#include "RenderManager.h"
#include "RenderResource.h"
#include "RHIGlobals.h"


namespace Lumina
{
    template<typename InitializerType, typename RHIRefType>
    class TStaticRHIRef
    {
    public:

        static RHIRefType GetRHI()
        {
            return InitializerType::CreateRHI();
        }
         
    };
    

    template<bool Filter=true,
    ESamplerAddressMode AddressU=ESamplerAddressMode::Clamp,
    ESamplerAddressMode AddressV=ESamplerAddressMode::Clamp,
    ESamplerAddressMode AddressW=ESamplerAddressMode::Clamp>
    class TStaticRHISampler : public TStaticRHIRef<TStaticRHISampler<Filter, AddressU, AddressV, AddressW>, FRHISamplerRef>
    {
    public:

        static FRHISamplerRef CreateRHI()
        {
            FSamplerDesc Desc;
            Desc.SetAllFilters(Filter);
            Desc.AddressU = AddressU;
            Desc.AddressV = AddressV;
            Desc.AddressW = AddressW;

            return GRenderContext->CreateSampler(Desc);
        }
    };
    
}
