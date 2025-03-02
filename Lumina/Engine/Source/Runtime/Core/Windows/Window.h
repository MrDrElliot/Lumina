#pragma once

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

#include "WindowTypes.h"
#include "Core/Functional/Function.h"
#include "Core/Math/Math.h"


namespace Lumina
{
	
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

		const FIntVector2D& GetExtent() const { return Specs.Extent; }
		uint32 GetWidth() const { return Specs.Extent.X; }
		uint32 GetHeight() const { return Specs.Extent.Y; }

		static void WindowResizeCallback(GLFWwindow* window, int width, int height);

	
	private:

		GLFWwindow* Window = nullptr;
		bool bInitialized = false;
		FWindowSpecs Specs;
	};

	namespace Windowing
	{
		FWindow* GetPrimaryWindowHandle();
		void SetPrimaryWindowHandle(FWindow* InWindow);
	}
	
}
