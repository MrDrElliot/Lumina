#pragma once

#include <memory>
#include <string>
#include "LayerStack.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Source/Runtime/Events/Event.h"
#include "Source/Runtime/Thread/RenderThread.h"


namespace Lumina
{
	class FImage;
	class FImGuiLayer;
	class LScene;
	struct FWindowSpecs;
	class FWindow;
	class FRenderContext;

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
		int Close();
		
		virtual void OnInit();
		virtual void OnShutdown();
		
		void CreateApplicationWindow(const FWindowSpecs& InSpecs);

		virtual void PreFrame();
		virtual void PostFrame();
		
		void PushLayer(std::shared_ptr<FLayer> InLayer);
		void PushOverlay(std::shared_ptr<FLayer> InLayer);
		void PopLayer(std::shared_ptr<FLayer> InLayer);
		void PopOverlay(std::shared_ptr<FLayer> InLayer);
		void InitImGuiLayer();
		virtual void RenderImGui();

		void OnEvent(FEvent& Event);
		
		static FApplication& Get() { return *Instance; }

		static FWindow& GetWindow() { return *Instance->Window;  }
	
	private:

		inline bool IsMinimized() const { return bMinimized; }
		inline bool ShouldExit() const;

	
	private:


		FApplicationSpecs AppSpecs;
		bool bRunning = true;
		bool bMinimized = false;

	private:

		/* Application Instance */
		static FApplication* Instance;
		
		/* Application Window */
		std::shared_ptr<FWindow> Window;

		/* Layer Stack */
		FLayerStack LayerStack;

		/* Im Gui Layer */
		std::shared_ptr<FImGuiLayer> ImGuiLayer;
		
	};

	/* Implemented by client */
	static std::unique_ptr<Lumina::FApplication> CreateApplication(int argc, char** argv);
}
