#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <crtdbg.h>
#include <assert.h>
#endif

#include <memory>



extern std::unique_ptr<Lumina::FApplication> Lumina::CreateApplication(int argc, char** argv);

inline bool gApplicationRunning = true;


inline int GuardedMain(int argc, char** argv)
{
#if LE_PLATFORM_WINDOWS
	SetUnhandledExceptionFilter(ExceptionHandler);
#endif
	// Enable debug heap allocations
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	int Exit = 0;
	while (gApplicationRunning)
	{
		std::unique_ptr<Lumina::FApplication> App = Lumina::CreateApplication(argc, argv);
		App->Run();
		Exit = App->Close();

		gApplicationRunning = false;
	}

	_CrtDumpMemoryLeaks();

	return Exit;
}

#if LE_PLATFORM_WINDOWS

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return GuardedMain(__argc, __argv);
}

#endif

int main(int argc, char** argv)
{
	return GuardedMain(argc, argv);
}