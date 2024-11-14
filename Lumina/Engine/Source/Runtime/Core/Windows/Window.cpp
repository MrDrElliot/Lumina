#include "pch.h"

#include "Window.h"

#include "Core/Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Renderer.h"


namespace
{
	void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}
}


namespace Lumina
{
	FWindow::FWindow(const FWindowSpecs& InSpecs)
	{
		Specs = InSpecs;
		Window = nullptr;
	}
	
	FWindow::~FWindow()
	{
	}

	void FWindow::Init()
	{
		if (!bInitialized)
		{
			LOG_TRACE("Initializing Window: {0} (Width: {1}p Height: {2}p)", Specs.Title, Specs.Width, Specs.Height);

			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //@TODO Temp until swapchain resizing is fixed.

			Window = glfwCreateWindow((int)Specs.Width, (int)Specs.Height, Specs.Title.c_str(), nullptr, nullptr);
		}
		else
		{
			LOG_ERROR("Init called on a window that's already been initialized!");
		}
	}

	void FWindow::OnUpdate(double DeltaTime)
	{
		glfwPollEvents();
	}

	void FWindow::Shutdown()
	{
		glfwDestroyWindow(Window);
	}

	bool FWindow::IsMinimized() const
	{
		int w, h;
		glfwGetWindowSize(Window, &w, &h);

		return (w == 0 || h == 0);
	}

	void FWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;
			
		WindowResizeEvent event(width, height);
		data.EventCallback(event);
	}

	void FWindow::SetEventCallback(const std::function<void(FEvent&)>& Callback)
	{
		Specs.EventCallback = Callback;

		glfwSetWindowUserPointer(Window, &Specs);
			
		glfwSetWindowSizeCallback(Window, WindowResizeCallback);
		
		glfwSetWindowCloseCallback(Window, [](GLFWwindow* window)
		{
			FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetDropCallback(Window, [](GLFWwindow* window, int path_count, const char* paths[])
		{
			FWindowSpecs& Data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
			WindowDropEvent event(path_count, paths);
			Data.EventCallback(event);
		});
		
		glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);

			KeyCode KeyCode = (uint16)key;
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(KeyCode, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(KeyCode);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(KeyCode, true);
					data.EventCallback(event);
					break;
				}
			default: ;
			}
		});
		
		glfwSetCharCallback(Window, [](GLFWwindow* window, unsigned int keycode)
		{
			FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
		
			KeyTypedEvent event((uint16)keycode);
			data.EventCallback(event);
		});
		
		glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			FWindowSpecs& data = *(FWindowSpecs*)glfwGetWindowUserPointer(window);
		
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event((uint16)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event((uint16)button);
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
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs)
	{
		return new FWindow(InSpecs);
	}

	

}
