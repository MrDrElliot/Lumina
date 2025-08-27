#pragma once

#include "Core/Engine/Engine.h"
#include "Core/Utils/CommandLineParser.h"

namespace Lumina
{
	enum class LUMINA_API EApplicationFlags : uint32
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
	
	class LUMINA_API FApplication
	{
	public:

		FApplication(const FString& InApplicationName = "Unnamed Application", uint32 AppFlags = 0);
		virtual ~FApplication() = default;

		static FApplication& Get() { return *Instance; }

		int32 Run(int argc, char** argv);

		virtual bool ApplicationLoop() = 0;
		virtual bool Initialize(int argc, char** argv) = 0;
		virtual void Shutdown() = 0;

		virtual void RenderDeveloperTools(const FUpdateContext& UpdateContext) { }

		bool HasAnyFlags(EApplicationFlags Flags);

		void WindowResized(const FIntVector2D& Extent);
		virtual void OnWindowResized(const FIntVector2D& Extent) { }


	protected:

		virtual FEngine* CreateEngine() = 0;
		
	private:

		void PreInitStartup();
		bool CreateApplicationWindow();
		bool FatalError(const FString& Error);
		
		bool ShouldExit();
		
	protected:

		FWindow*					Window = nullptr;
		
		FString ApplicationName =	"Unnamed Application";
		
		static FApplication*		Instance;
		
		uint32						ApplicationFlags = 0;

	public:

		static FCommandLineParser	CommandLine;
	};

	/* Implemented by client */
	static FApplication* CreateApplication(int argc, char** argv);
}
