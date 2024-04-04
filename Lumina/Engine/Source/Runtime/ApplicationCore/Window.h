#pragma once

#include <cstdint>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <memory>
#include <GLFW/glfw3.h>

#include "Source/Runtime/Memory/Memory.h"


namespace Lumina
{
	class FRendererContext;

	struct FWindowSpecs
	{
		std::string Title = "Lumina";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class FWindow
	{
	public:
		static FWindow* Create(const FWindowSpecs& InSpecs);

		FWindow(const FWindowSpecs& InSpecs, bool bInit = false);
		virtual ~FWindow();


		virtual void Init();
		virtual  std::shared_ptr<FRendererContext> CreateRenderContext();
		virtual void OnUpdate(float DeltaTime);



	private:

		virtual void OnShutdown();


	private:

		GLFWwindow* Window;
		
		std::shared_ptr<FRendererContext> RendererContext;

		bool bInitialized = false;
		
		FWindowSpecs Specs;

	};
}
