
#include "Sandbox.h"
#include "EntryPoint.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Math/Transform.h"
#include "Input/Input.h"
#include "Scene/Entity/Systems/EntitySystem.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/Entity.h"
#include "Scene/Entity/Components/CameraComponent.h"
#include "Scene/Subsystems/FCameraManager.h"

using namespace Lumina;


void FSandbox::CreateEngine()
{
	Engine = FMemory::New<FSandboxEngine>();
	Engine->Initialize(this);
	Engine->SetUpdateCallback([this] (const FUpdateContext& UpdateContext)
	{
		EngineLoopCallback(UpdateContext);
	});
}

void FSandbox::EngineLoopCallback(const FUpdateContext& UpdateContext)
{
	FRenderManager* RenderManager = UpdateContext.GetSubsystem<FRenderManager>();
	IRenderContext* RenderContext = RenderManager->GetRenderContext();
	ICommandList* CommandList = RenderContext->GetCommandList();

	struct TestData
	{
		glm::vec4 Color;
	};
	
	// Here we setup a buffer that is used as a constant view.
	FRHIBufferDesc Buffer;
	Buffer.Size = sizeof(TestData);
	Buffer.Usage.SetFlag(BUF_UniformBuffer);
	FRHIBufferRef BufferRef = RenderContext->CreateBuffer(Buffer);
	

	// Here we upload a float (time) to the buffer so it can be read from in the shader.
	TestData Test;
	Test.Color = glm::vec4(0.0f, 0.0f, 1.0f, glfwGetTime());
	CommandList->UploadToBuffer(BufferRef, &Test, 0, sizeof(TestData));

	// Here we setup a storage image, (Unordered Access View), the compute shader will write to this.
	FRHIImageDesc Image;
	Image.Extent = GEngine->GetEngineViewport()->GetSize();
	Image.Format = EImageFormat::RGBA32_UNORM;
	Image.Flags.SetFlag(EImageCreateFlags::Storage);
	FRHIImageRef ImageRef = RenderContext->CreateImage(Image);

	// Here we define the layout of the shader.
	// Binding 0 is an Unordered Access View texture.
	// Binding 1 is a uniform buffer (not written to, only read).
	FBindingLayoutDesc LayoutDesc;
	LayoutDesc.AddItem({0, ERHIBindingResourceType::Texture_UAV, 1});
	LayoutDesc.AddItem({1, ERHIBindingResourceType::Buffer_CBV, 1});
	LayoutDesc.Index = 0;
	LayoutDesc.SetVisibility(ERHIShaderType::Compute);
	FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(LayoutDesc);

	// Here we define the actual resources that will be bound to the cooresponding FRHIBindingLayoutRef.
	// 0 = Image
	// 1 = Buffer
	FBindingSetDesc SetDesc;
	SetDesc.AddItem(FBindingSetItem::TextureUAV(0, ImageRef));
	SetDesc.AddItem(FBindingSetItem::BufferCBV(1, BufferRef));
	FRHIBindingSetRef SetLayout = RenderContext->CreateBindingSet(SetDesc, Layout);

	// Here we finalize the compute pipeline, giving it the shader we want, and the final binding. 
	FComputePipelineDesc Desc;
	Desc.SetComputeShader(RenderContext->GetShaderLibrary()->GetShader("Test.comp").As<FRHIComputeShader>());
	Desc.AddBindingLayout(Layout);
	FRHIComputePipelineRef Pipeline = RenderContext->CreateComputePipeline(Desc);
	

	// We finally bind the final state of the pipeline.
	CommandList->SetComputePipeline(Pipeline);

	// Here we bind the set to the pipeline that we just bound (which is tracked internally).
	CommandList->BindBindingSet(SetLayout, ERHIBindingPoint::Compute);

	// Finally we dispatch the compute shader, giving it the proper amount of work groups.
	CommandList->Dispatch(ImageRef->GetSizeX() / 16, ImageRef->GetSizeY() / 16, 1);

	// Since we wrote to a transient storage image, we now want to copy/blit that image back to the render target so we can see it.
	CommandList->CopyImage(ImageRef, GEngine->GetEngineViewport()->GetRenderTarget());
	
	
}

bool FSandbox::ApplicationLoop()
{
	return true;
}

bool FSandbox::Initialize()
{
	return true;
}

void FSandbox::Shutdown()
{
}


Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv)
{
	return new FSandbox();
}