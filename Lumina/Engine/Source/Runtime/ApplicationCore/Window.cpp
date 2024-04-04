#include "Window.h"

#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/RendererContext.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanRendererContext.h"

namespace
{
	void GLFWErrorCallback(int error, const char* description)
	{
		LE_LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}
}


namespace Lumina
{

	FWindow::FWindow(const FWindowSpecs& InSpecs, bool bInit)
	{
		Specs = InSpecs;

	}

	FWindow::~FWindow()
	{
	}

	void FWindow::Init()
	{
		if (!bInitialized)
		{
			LE_LOG_INFO("Initializing Window: {0} {1} {2}", Specs.Title, Specs.Width, Specs.Height);

			glfwInit();
			
			glfwSetErrorCallback(GLFWErrorCallback);
			
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			GLFWmonitor* PrimaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* VidMode = glfwGetVideoMode(PrimaryMonitor);

			Window = glfwCreateWindow(Specs.Width, Specs.Height, Specs.Title.c_str(), nullptr, nullptr);

			RendererContext = CreateRenderContext();

				
			return;
		}
		
		LE_LOG_ERROR("Init called on a window that's already been initialized!");
	}

	 std::shared_ptr<FRendererContext> FWindow::CreateRenderContext()
	{
		std::shared_ptr<FRendererContext> NewContext = FRendererContext::Create();
		NewContext->Init();
		return NewContext;
	}

	void FWindow::OnUpdate(float DeltaTime)
	{
		glfwPollEvents();
	}

	void FWindow::OnShutdown()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs)
	{
		return new FWindow(InSpecs);
	}
}
