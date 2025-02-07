#pragma once

#include <memory>
#include "Events/Event.h"
#include "Subsystems/Subsystem.h"
#include "Core/Delegates/Delegate.h"
#include "Core/Engine/Engine.h"


namespace Lumina
{
	class FLayer;
	class WindowSubsystem;
	class FAssetManager;
	class FBuffer;
	class FDescriptorSet;
	class FImage;
	class FScene;
	struct FWindowSpecs;
	class FWindow;
	class FRenderContext;

	enum class EApplicationFlags : uint32
	{
		DevelopmentTools =		1 << 0,
	};

	constexpr EApplicationFlags operator|(EApplicationFlags lhs, EApplicationFlags rhs)
	{
		return static_cast<EApplicationFlags>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
	}

	constexpr EApplicationFlags operator&(EApplicationFlags lhs, EApplicationFlags rhs)
	{
		return static_cast<EApplicationFlags>(static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
	}
	
	class FApplication
	{
	public:

		FApplication() = default;
		FApplication(FString InApplicationName = "Unnamed Application", uint32 AppFlags = 0);
		virtual ~FApplication();

		static FApplication& Get() { return *Instance; }

		int32 Run();

		virtual bool ApplicationLoop() = 0;
		
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void RenderDeveloperTools(const FUpdateContext& UpdateContext) { }
		virtual void OnEvent(FEvent& Event) { }

		bool HasAnyFlags(EApplicationFlags Flags);
		FWindow* GetWindow();
		
		


	protected:

		virtual void CreateEngine() = 0;
		
	private:
		
		bool CreateApplicationWindow();
		bool FatalError(const FString& Error);
		
		bool ShouldExit();
		
	protected:

		FWindow*					Window = nullptr;
		
		FString ApplicationName =	"Unnamed Application";
		
		static FApplication*		Instance;
		
		uint32						ApplicationFlags = 0;

		FEngine*					Engine;
		
	};

	/* Implemented by client */
	static FApplication* CreateApplication(int argc, char** argv);
}
