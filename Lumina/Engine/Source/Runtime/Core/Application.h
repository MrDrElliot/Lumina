#pragma once

#include <memory>
#include <string>
#include "Events/Event.h"
#include "Subsystems/Subsystem.h"
#include "LayerStack.h"
#include "Delegates/Delegate.h"
#include "Memory/RefCounted.h"


namespace Lumina
{
	class FLayer;
	class WindowSubsystem;
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
		const char* Name = "Lumina";
		std::string WorkingDirectory;
		
		uint32 WindowWidth = 1920;
		uint32 WindowHeight = 1080;
		
		uint8 bWindowDecorated:1;
		uint8 bFullscreen:1;
		uint8 bVSync:1;
		uint8 bRenderImGui:1;

	};

	struct FApplicationStats
	{
		double DeltaTime = 1.0f / 60.0f;
		double LastFrameTime = 0.0f;
		double CurrentFrameTime = 60.0f;
		
		uint32 FPS;
		uint64 FrameCount = 0;

	};

	struct FCoreDelegates
	{
		static TMulticastDelegate<void>		OnEngineInit;
		static TMulticastDelegate<double>	OnEngineUpdate;
		static TMulticastDelegate<void>		PreEngineShutdown;
	};
	
	class FApplication
	{
	public:

		FApplication(const FApplicationSpecs& InAppSpecs);
		virtual ~FApplication();

		static FApplication& Get() { return *Instance; }

		void Run();
		int Close();
		
		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnShutdown();
		
		void PreFrame();
		void PostFrame();

		virtual void UpdateLayerStack(double DeltaTime);
		void PushLayer(const TRefPtr<FLayer>& InLayer);
		void PushOverlay(const TRefPtr<FLayer>& InLayer);
		void PopLayer(const TRefPtr<FLayer>& InLayer);
		void PopOverlay(const TRefPtr<FLayer>& InLayer);
		

		void RenderImGui(double DeltaTime);
		virtual void OnEvent(FEvent& Event);
		
		static FApplicationSpecs GetSpecs() { return Instance->AppSpecs;  }
		static FApplicationStats GetStats() { return Instance->Stats; }
		static double GetDeltaTime() { return Instance->Stats.DeltaTime; }
		static FWindow& GetWindow();
		static std::shared_ptr<LScene> GetActiveScene() { return Get().mScene; }
		
		void SetCurrentScene(std::shared_ptr<LScene> InScene);
		
		
		template<typename T, typename... Args>
		T* AddSubsystem(Args&&... args)
		{
			return ApplicationSubsystems.AddSubsystem<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		T* GetSubsystem()
		{
			return ApplicationSubsystems.GetSubsystem<T>();
		}

		template<typename T>
		TRefPtr<T> GetLayerByType()
		{
			return LayerStack.GetLayerByType<T>();
		}
	
	private:

		bool ShouldExit();
	
	
	private:

		std::shared_ptr<LScene> mScene;
		
		FApplicationStats Stats;
		FApplicationSpecs AppSpecs;

		bool bRunning = true;
		
		/* Application Instance */
		static FApplication* Instance;

		/* Layer Stack */
		FLayerStack LayerStack;
		
		/** Container of application subsystems. */
		SubsystemManager ApplicationSubsystems;
		
	};

	/* Implemented by client */
	static std::unique_ptr<FApplication> CreateApplication(int argc, char** argv);
}
