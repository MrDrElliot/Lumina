#include "SkyboxRenderPass.h"

#include "RenderGraphResources.h"
#include "Core/Profiler/Profile.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderResource.h"
#include "Scene/SceneRenderTypes.h"

namespace Lumina
{

    void FSkyboxRenderPass::Execute(FRenderGraphScope& Scope)
    {
        LUMINA_PROFILE_SCOPE();
        
        
        ICommandList* CommandList = Scope.GetCommandList();
        IRenderContext* RenderContext = Scope.GetRenderContext();

        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("Skybox.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("Skybox.frag");
        if (!VertexShader || !PixelShader)
        {
            return;
        }

        FRHIImageRef ColorAttachment = Scope.Get<PrimaryRenderTargetTag>().As<FRHIImage>();
        FRHIImageRef DepthAttachment = Scope.Get<DepthBufferTargetTag>().As<FRHIImage>();
        FRHIBufferRef SceneGlobalData = Scope.Get<SceneGlobalDataTag>().As<FRHIBuffer>();
        FRHIImageRef CubeMap = Scope.Get<CubeMapImageTag>().As<FRHIImage>();
        
        FBindingLayoutItem Item;
        Item.Size = sizeof(FSceneGlobalData);
        Item.Slot = 0;
        Item.Type = ERHIBindingResourceType::Buffer_CBV;
        
        FBindingLayoutDesc LayoutDesc;
        LayoutDesc.AddItem(Item);
        LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Vertex);
        FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(LayoutDesc);
        
        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneGlobalData));
        FRHIBindingSetRef Set = RenderContext->CreateBindingSet(SetDesc, Layout);
        
        
        FBindingLayoutItem ImageItem;
        ImageItem.Slot = 0;
        ImageItem.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutDesc ImageLayoutDesc;
        ImageLayoutDesc.AddItem(ImageItem);
        ImageLayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
        FRHIBindingLayoutRef Layout2 = RenderContext->CreateBindingLayout(ImageLayoutDesc);

        FBindingSetDesc SetDesc2;
        SetDesc2.AddItem(FBindingSetItem::TextureSRV(0, CubeMap));
        FRHIBindingSetRef Set2 = RenderContext->CreateBindingSet(SetDesc2, Layout2);
        

        FDepthStencilState DepthState;
        DepthState.SetDepthTestEnable(false);
        DepthState.SetDepthWriteEnable(false);
        
        FBlendState BlendState;
        FBlendState::RenderTarget RenderTarget;
        RenderTarget.EnableBlend();
        RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
        RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
        RenderTarget.SetSrcBlendAlpha(EBlendFactor::One);
        RenderTarget.SetDestBlendAlpha(EBlendFactor::Zero);
        BlendState.SetRenderTarget(0, RenderTarget);

        FRasterState RasterState;
        RasterState.SetCullNone();
        RasterState.SetDepthClipEnable(false);

        FRenderState RenderState;
        RenderState.SetBlendState(BlendState);
        RenderState.SetRasterState(RasterState);
        RenderState.SetDepthStencilState(DepthState);
        
        FGraphicsPipelineDesc Desc;
        Desc.SetRenderState(RenderState);
        Desc.AddBindingLayout(Layout);
        Desc.AddBindingLayout(Layout2);
        Desc.SetVertexShader(VertexShader);
        Desc.SetPixelShader(PixelShader);

        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
        
        CommandList->SetGraphicsPipeline(Pipeline);

        CommandList->BindBindingSets({Set, Set2}, ERHIBindingPoint::Graphics);

        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(ColorAttachment)
        .SetColorLoadOp(ERenderLoadOp::Load)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)
                
        .SetDepthAttachment(DepthAttachment)
        .SetDepthClearValue(0.0f)
        .SetDepthLoadOp(ERenderLoadOp::Clear)
        .SetDepthStoreOp(ERenderStoreOp::Store)
                
        .SetRenderArea(ColorAttachment->GetExtent());
        CommandList->BeginRenderPass(BeginInfo);

        CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)ColorAttachment->GetSizeX(), (float)ColorAttachment->GetSizeY(), 1.0f);
        CommandList->SetScissorRect(0, 0, ColorAttachment->GetSizeX(), ColorAttachment->GetSizeY());

        CommandList->Draw(36, 1, 0, 0);
        
        CommandList->EndRenderPass();
    }
}
