#include "Window.h"

#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/RenderContext.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRenderContext.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanSwapChain.h"

namespace
{
	void GLFWErrorCallback(int error, const char* description)
	{
		LE_LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}
}


namespace Lumina
{
	FWindow::FWindow(const FWindowSpecs& InSpecs, FVulkanSwapChain* InSwapChain)
	{
		Specs = InSpecs;
		SwapChain = InSwapChain;
	}
	
	FWindow::~FWindow()
	{
		
	}

	void FWindow::Init()
	{
		if (!bInitialized && SwapChain)
		{
			LE_LOG_INFO("Initializing Window: {0} {1} {2}", Specs.Title, Specs.Width, Specs.Height);

			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			
			Window = glfwCreateWindow(Specs.Width, Specs.Height, Specs.Title.c_str(), nullptr, nullptr);

			return;
		}
		
		LE_LOG_ERROR("Init called on a window that's already been initialized!");
	}

	void FWindow::OnUpdate(float DeltaTime)
	{
		glfwPollEvents();

		FApplication::Get().GetRenderContext<FVulkanRenderContext>()->Draw(DeltaTime);
	}

	void FWindow::Shutdown()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs, FVulkanSwapChain* InSwapChain)
	{
		FWindow* NewWindow = new FWindow(InSpecs, InSwapChain);
		return NewWindow;
	}

}
