#include "pch.h"

#include "Window.h"
#include "Core/Application/Application.h"
#include "Events/ApplicationEvent.h"
#include "Platform/Platform.h"
#include "Renderer/RHIIncl.h"



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
	
	FWindow::~FWindow()
	{
		Assert(Window == nullptr);
	}

	void FWindow::Init()
	{
		if (LIKELY(!bInitialized))
		{
			glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			//glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

			
			// Create the window
			Window = glfwCreateWindow(800, 400, Specs.Title.c_str(), nullptr, nullptr);
			if (GLFWmonitor* currentMonitor = GetCurrentMonitor(Window))
			{
				// Get monitor dimensions
				int monitorX, monitorY, monitorWidth, monitorHeight;
				glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

				// Check if Specs.Width or Specs.Height is 0 and set them to the monitor size - 1
				if (Specs.Extent.X == 0 || Specs.Extent.X >= monitorWidth)
				{
					Specs.Extent.X = monitorWidth/1.15;
				}
				if (Specs.Extent.Y == 0 || Specs.Extent.Y >= monitorHeight)
				{
					Specs.Extent.Y = monitorHeight/1.15;
				}
				

				// Log the window's initialization details
				LOG_TRACE("Initializing Window: {0} (Width: {1}p Height: {2}p)", Specs.Title, Specs.Extent.X, Specs.Extent.Y);

				// Update the window size after adjustment
				glfwSetWindowSize(Window, Specs.Extent.X, Specs.Extent.Y);
			}
			
			glfwSetWindowUserPointer(Window, &Specs);
			glfwSetWindowSizeCallback(Window, WindowResizeCallback);
		}
	}
	
	void FWindow::Shutdown()
	{
		glfwDestroyWindow(Window);
		Window = nullptr;
		
		glfwTerminate();
	}

	void FWindow::ProcessMessages()
	{
		glfwPollEvents();
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
		data.Extent.X = width;
		data.Extent.Y = height;

		data.Context.ResizeCallback(data.Extent);
		
	}
	
	FWindow* FWindow::Create(const FWindowSpecs& InSpecs)
	{
		return new FWindow(InSpecs);
	}

	namespace Windowing
	{
		FWindow* PrimaryWindow = nullptr;
		
		FWindow* GetPrimaryWindowHandle()
		{
			Assert(PrimaryWindow != nullptr);
			return PrimaryWindow;
		}

		void SetPrimaryWindowHandle(FWindow* InWindow)
		{
			Assert(PrimaryWindow == nullptr);
			PrimaryWindow = InWindow;
		}
	}
}
