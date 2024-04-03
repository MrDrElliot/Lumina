#pragma once

#include <memory>
#include <string>
#include "LayerStack.h"
#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Lumina
{

	struct FApplicationSpecs
	{
		std::string Name = "Lumina";
		uint32_t WindowWidth = 1600;
		uint32_t WindowHeight = 900;
		bool bWindowDecorated = false;
		bool bFullscreen = false;
		bool bVSync = true;
		std::string WorkingDirectory;
	};
	
	class FApplication
	{
	public:

		FApplication(const FApplicationSpecs& InAppSpecs);
		virtual ~FApplication();
		

		void Run();
		void Close();


		virtual void OnInit();
		virtual void OnShutdown();

		void PushLayer(FLayer* InLayer);
		void PushOverlay(FLayer* InLayer);
		void PopLayer(FLayer* InLayer);
		void PopOverlay(FLayer* InLayer);



	private:

		inline bool IsMinimized() const { return bMinimized; }
		inline bool ShouldExit() const { return !bRunning; }

	private:

		std::unique_ptr<FWindow> Window;

		FApplicationSpecs AppSpecs;
		bool bRunning = true;
		bool bMinimized = false;

	private:


		FLayerStack LayerStack;

		static FApplication* Instance;
	};

	/* Implemented by client */
	static FApplication* CreateApplication(int argc, char** argv);
}
