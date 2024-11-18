#include "pch.h"

#include "Window.h"

#include "Core/Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Renderer.h"
#include "Renderer/Swapchain.h"


namespace
{
	void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CRITICAL("GLFW Error: {0} | {1}", error, description);
	}
}


namespace Lumina
{

	GLFWmonitor* GetCurrentMonitor(GLFWwindow* window)
	{
		int windowX, windowY, windowWidth, windowHeight;
		glfwGetWindowPos(window, &windowX, &windowY);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

		GLFWmonitor* bestMonitor = nullptr;
		int maxOverlap = 0;

		for (int i = 0; i < monitorCount; ++i)
		{
			int monitorX, monitorY, monitorWidth, monitorHeight;
			glfwGetMonitorWorkarea(monitors[i], &monitorX, &monitorY, &monitorWidth, &monitorHeight);

			int overlapX = std::max(0, std::min(windowX + windowWidth, monitorX + monitorWidth) - std::max(windowX, monitorX));
			int overlapY = std::max(0, std::min(windowY + windowHeight, monitorY + monitorHeight) - std::max(windowY, monitorY));
			int overlapArea = overlapX * overlapY;

			if (overlapArea > maxOverlap)
			{
				maxOverlap = overlapArea;
				bestMonitor = monitors[i];
			}
		}

		return bestMonitor;
	}


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
			//glfwWindowHint(GLFW_DECORATED, false);

			if (Specs.bFullscreen)
			{
				glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			}


			Window = glfwCreateWindow((int)Specs.Width, (int)Specs.Height, Specs.Title.c_str(), nullptr, nullptr);
			if (Window)
			{
				GLFWmonitor* currentMonitor = GetCurrentMonitor(Window);

				if (currentMonitor)
				{
					int monitorX, monitorY, monitorWidth, monitorHeight;
					glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

					// Check and adjust the window dimensions
					if (Specs.Width >= monitorWidth)
					{
						Specs.Width = monitorWidth - 1; // Shrink width by 1 pixel if it's too large
					}
					if (Specs.Height >= monitorHeight)
					{
						Specs.Height = monitorHeight - 1; // Shrink height by 1 pixel if it's too large
					}

					// Update the window size after adjustment
					glfwSetWindowSize(Window, Specs.Width, Specs.Height);
				}
			}
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

		FRenderer::GetSwapchain()->SetSwapchainDirty();
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
