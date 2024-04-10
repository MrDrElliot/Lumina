#pragma once

#include <memory>
#include <string>
#include "LayerStack.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Source/Runtime/Thread/GameThread.h"
#include "Source/Runtime/Thread/RenderThread.h"


namespace Lumina
{
	class FImGuiLayer;
}

namespace Lumina
{
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
		void Close();
		
		virtual void OnInit();
		virtual void OnShutdown();
		
		void CreateApplicationWindow(const FWindowSpecs& InSpecs);

		virtual void CheckWindowResized();
		
		void PushLayer(FLayer* InLayer);
		void PushOverlay(FLayer* InLayer);
		void PopLayer(FLayer* InLayer);
		void PopOverlay(FLayer* InLayer);
		void InitImGuiLayer();
		virtual void RenderImGui();



		
		
		static FApplication& Get() { return *Instance; }

		FWindow& GetWindow() { return *Window;  }
		

		FRenderThread* GetRenderThread() const { return RenderThread.get(); }
		std::thread::id GetRenderThreadID() const { return RenderThread->GetThreadID(); }

		
		
		template<typename T>
		T* GetRenderContext()
		{
			//@ TODO Not allowed?
			//static_assert(std::is_base_of<FRenderContext, T>::Value, "T Must be derived from FRenderContext");
			return dynamic_cast<T*>(RenderContext);
		}
	
	private:

		inline bool IsMinimized() const { return bMinimized; }
		inline bool ShouldExit() const { return !bRunning; }

	private:


		FApplicationSpecs AppSpecs;
		bool bRunning = true;
		bool bMinimized = false;

	private:

		/* Application Instance */
		static FApplication* Instance;
		

		/* Rendering Context */
		FRenderContext* RenderContext;
		
		/* Application Window */
		std::unique_ptr<FWindow> Window;

		/* Layer Stack */
		FLayerStack LayerStack;

		FImGuiLayer* ImGuiLayer;


		
		/* Render Thread */
		std::unique_ptr<FRenderThread> RenderThread;
		
	};

	/* Implemented by client */
	static FApplication* CreateApplication(int argc, char** argv);
}
