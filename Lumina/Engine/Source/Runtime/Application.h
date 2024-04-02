#pragma once

#include <string>

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

		FApplication() {}
		FApplication(const FApplicationSpecs& InAppSpecs);
		virtual ~FApplication();
		

		void Run();
		void Close();


		virtual void OnInit();
		virtual void OnShutdown();


	private:

		inline bool ShouldExit() const { return !bRunning; }

	private:

		FApplicationSpecs AppSpecs;
		bool bRunning = true;
		bool bMinimized = false;
	};

	/* Implemented by client */
	static FApplication* CreateApplication(int argc, char** argv);
}
