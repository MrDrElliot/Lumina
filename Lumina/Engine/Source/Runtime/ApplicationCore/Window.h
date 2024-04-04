#pragma once

#include <cstdint>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Source/Runtime/RHI/RendererContext.h"
#include "Source/Runtime/RHI/Vulkan/VulkanSwapChain.h"

namespace Lumina
{

	struct FWindowSpecs
	{
		std::string Title = "Lumina";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class FWindow
	{
	public:
		static FWindow* Create(const FWindowSpecs& InSpecs, bool bInit = false);

		FWindow(const FWindowSpecs& InSpecs, bool bInit = false);
		virtual ~FWindow();


		virtual void Init();
		virtual void OnUpdate(float DeltaTime);

		LRendererContext* GetRendererContext() const { return RendererContext; }
		FVulkanSwapChain* GetSwapChain() const { return SwapChain; }

	private:

		virtual void OnShutdown();


	private:

		GLFWwindow* Window;

		bool bInitialized = false;
		
		FWindowSpecs Specs;

		LRendererContext* RendererContext;
		FVulkanSwapChain* SwapChain;
		
	};
}
