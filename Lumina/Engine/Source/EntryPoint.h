#pragma once

#ifdef _DEBUG
#ifdef LE_PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <crtdbg.h>
#endif
#endif

#include <memory>



extern Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv);

inline bool gApplicationRunning = true;


inline int GuardedMain(int argc, char** argv)
{
#if LE_PLATFORM_WINDOWS
	SetUnhandledExceptionFilter(ExceptionHandler);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	int Exit = 0;
	while (gApplicationRunning)
	{
		Lumina::FApplication* App = Lumina::CreateApplication(argc, argv);
		App->Run();

		delete App;
		gApplicationRunning = false;
	}
	
#ifdef LE_PLATFORM_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
	
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

