#pragma once

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

#include "WindowTypes.h"


namespace Lumina
{
	

	class FRendererContext;
	
	class FWindow
	{
	public:
		
		static FWindow* Create(const FWindowSpecs& InSpecs);

		FWindow(const FWindowSpecs& InSpecs)
			:Specs(InSpecs)
		{}
		 
		virtual ~FWindow();


		void Init();
		void Shutdown();
		void ProcessMessages();

		GLFWwindow* GetWindow() const { return Window; }
		bool IsMinimized() const;

		uint32 GetWidth() const { return Specs.Width; }
		uint32 GetHeight() const { return Specs.Height; }

		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		void SetEventCallback(const std::function<void(FEvent&)>& Callback);

	
	private:

		GLFWwindow* Window = nullptr;
		bool bInitialized = false;
		FWindowSpecs Specs;
	};

	namespace Windowing
	{
		const FWindow* GetPrimaryWindowHandle();
		void SetPrimaryWindowHandle(const FWindow* InWindow);
	}
	
}
