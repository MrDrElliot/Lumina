#include "Window.h"

#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/RHI/Vulkan/VulkanRendererContext.h"

namespace Lumina
{

	static void GLFWErrorCallback(int error, const char* description)
	{
		LE_LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}


	FWindow::FWindow(const FWindowSpecs& InSpecs, bool bInit)
	{
		Specs = InSpecs;

		if (bInit)
		{
			Init();
		}
	}

	FWindow::~FWindow()
	{
		OnShutdown();
	}

	void FWindow::Init()
	{
		if (!bInitialized)
		{
			LE_LOG_INFO("Initializing Window: {0} {1} {2}", Specs.Title, Specs.Width, Specs.Height);

			int bInitalized = glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);

			// @TODO FRenderer::Current() = ERendererAPI::Vulkan

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


			GLFWmonitor* PrimaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* VidMode = glfwGetVideoMode(PrimaryMonitor);

			Window = glfwCreateWindow(Specs.Width, Specs.Height, Specs.Title.c_str(), nullptr, nullptr);

			RendererContext = LRendererContext::Create(true);

			const LVulkanRendererContext* VulkanContext = dynamic_cast<LVulkanRendererContext*>(RendererContext);

			SwapChain = new FVulkanSwapChain(LVulkanRendererContext::GetInstance(), VulkanContext->GetDevice(), Window);
			SwapChain->InitSurface(Window);
			SwapChain->Create(&Specs.Width, &Specs.Height, true);
			
		}
	}

	void FWindow::OnUpdate(float DeltaTime)
	{
		glfwPollEvents();
		SwapChain->Present();
	}

	void FWindow::OnShutdown()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs, bool bInit)
	{
		return new FWindow(InSpecs, bInit);
	}
}
