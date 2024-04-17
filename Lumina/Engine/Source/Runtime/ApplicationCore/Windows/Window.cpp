#include "Window.h"

#include "backends/imgui_impl_vulkan.h"
#include "Source/Runtime/ApplicationCore/Application.h"
#include "Source/Runtime/Events/ApplicationEvent.h"
#include "Source/Runtime/Events/KeyEvent.h"
#include "Source/Runtime/Events/MouseEvent.h"
#include "Source/Runtime/Input/Input.h"
#include "Source/Runtime/Log/Log.h"

namespace
{
	void GLFWErrorCallback(int error, const char* description)
	{
		LE_LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}
}


namespace Lumina
{
	FWindow::FWindow(const FWindowSpecs& InSpecs)
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

			Window = glfwCreateWindow(Specs.Width, Specs.Height, Specs.Title.c_str(), nullptr, nullptr);

			glfwSetWindowUserPointer(Window, &Specs);
			
			glfwSetWindowSizeCallback(Window, WindowResizeCallback);

			glfwSetWindowCloseCallback(Window, [](GLFWwindow* window)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});
			
			glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			
				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, true);
						data.EventCallback(event);
						break;
					}
				default: ;
				}
			});
			
			glfwSetCharCallback(Window, [](GLFWwindow* window, unsigned int keycode)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			
				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});
			
			glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			
				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				default: ;
				}
			});
			
			glfwSetScrollCallback(Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});
			
			glfwSetCursorPosCallback(Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
				
			
			return;
				
		}
		
		LE_LOG_ERROR("Init called on a window that's already been initialized!");
	}

	void FWindow::OnUpdate(float DeltaTime)
	{
		glfwPollEvents();
	}

	void FWindow::Shutdown()
	{
		glfwDestroyWindow(Window);
		glfwTerminate();
	}

	void FWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		LE_LOG_INFO("Resizing Window To: {0} {1}", width, height);
		
		FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;
			
		WindowResizeEvent event(width, height);
		data.EventCallback(event);
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs)
	{
		FWindow* NewWindow = new FWindow(InSpecs);
		return NewWindow;
	}

	

}
