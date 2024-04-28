#pragma once

#include <memory>
#include <string>
#include "LayerStack.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Source/Runtime/Events/Event.h"
#include "Source/Runtime/Thread/RenderThread.h"
#include "Windows/Window.h"


namespace Lumina
{
	class FAssetManager;
	class FBuffer;
	class FDescriptorSet;
	class FImage;
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
		bool bRenderImGui = true;
		std::string WorkingDirectory;
	};

	struct FApplicationStats
	{
		double DeltaTime = 1.0f / 60.0f;
		double LastFrameTime = 0.0f;
		double CurrentFrameTime = 60.0f;
		
		uint32_t FPS;
		int FrameCount = 0;

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

		virtual void OnEvent(FEvent& Event);
		
		static FApplication& Get() { return *Instance; }
		
		static FApplicationStats GetStats() { return Instance->Stats; }
		static double GetDeltaTime() { return Instance->Stats.DeltaTime; }

		static FWindow& GetWindow() { return *Instance->Window;  }
		static std::shared_ptr<LScene> GetActiveScene() { return Instance->ActiveScene; }

		static std::shared_ptr<FAssetManager> GetAssetManager() { return Instance->AssetManager; }
	
	private:

		inline bool IsMinimized() const { return Window->IsMinimized(); }
		inline bool ShouldExit() const;
		inline bool ShouldRenderImGui() const { return AppSpecs.bRenderImGui; }
	
	
	private:

		std::shared_ptr<LScene> ActiveScene;

		FApplicationStats Stats;
		FApplicationSpecs AppSpecs;
		bool bRunning = true;

	private:

		/* Application Instance */
		static FApplication* Instance;
		
		/* Application Window */
		std::shared_ptr<FWindow> Window;

		/* Layer Stack */
		FLayerStack LayerStack;

		std::shared_ptr<FAssetManager> AssetManager;
		
	};

	/* Implemented by client */
	static std::unique_ptr<Lumina::FApplication> CreateApplication(int argc, char** argv);
}
