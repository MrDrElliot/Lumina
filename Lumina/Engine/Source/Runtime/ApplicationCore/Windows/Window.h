#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "WindowTypes.h"




namespace Lumina
{

	class FVulkanSwapChain;
	class FRendererContext;
	
	class FWindow
	{
	public:
		
		static FWindow* Create(const FWindowSpecs& InSpecs, FVulkanSwapChain* InSwapChain);

		FWindow(const FWindowSpecs& InSpecs, FVulkanSwapChain* InSwapChain);
		virtual ~FWindow();


		virtual void Init();
		virtual void OnUpdate(float DeltaTime);
		virtual void Shutdown();

		GLFWwindow* GetWindow() const { return Window; }
		FVulkanSwapChain* GetSwapChain() const { return SwapChain; }

		uint32_t GetWidth() const { return Specs.Width; }
		uint32_t GetHeight() const { return Specs.Height; }

		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		
	private:
	
	private:

		GLFWwindow* Window;
		FVulkanSwapChain* SwapChain;
		bool bInitialized = false;
		FWindowSpecs Specs;
	};
}
