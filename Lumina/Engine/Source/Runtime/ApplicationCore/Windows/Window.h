#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

#include "WindowTypes.h"
#include "Source/Runtime/Events/Event.h"


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
		void SetEventCallback(const std::function<void(FEvent&)>& Callback) { Specs.EventCallback = Callback; }

	
	private:

		GLFWwindow* Window;
		FVulkanSwapChain* SwapChain;
		bool bInitialized = false;
		FWindowSpecs Specs;
	};
}
