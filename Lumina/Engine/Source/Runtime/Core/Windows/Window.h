#pragma once

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

#include "WindowTypes.h"


namespace Lumina
{

	class FVulkanSwapChain;
	class FRendererContext;
	
	class FWindow
	{
	public:


		
		static FWindow* Create(const FWindowSpecs& InSpecs);

		FWindow(const FWindowSpecs& InSpecsn);
		virtual ~FWindow();


		virtual void Init();
		virtual void OnUpdate(double DeltaTime);
		virtual void Shutdown();

		GLFWwindow* GetWindow() const { return Window; }
		bool IsMinimized() const;

		uint32 GetWidth() const { return Specs.Width; }
		uint32 GetHeight() const { return Specs.Height; }

		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		void SetEventCallback(const std::function<void(FEvent&)>& Callback);

	
	private:

		GLFWwindow* Window;
		bool bInitialized = false;
		FWindowSpecs Specs;
	};
}