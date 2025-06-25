#pragma once


#ifdef _DEBUG
#ifdef LE_PLATFORM_WINDOWS
#include <crtdbg.h>
#endif
#endif

#include "Core/Application/ApplicationGlobalState.h"


extern Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv);

inline bool gApplicationRunning = true;


inline int GuardedMain(int argc, char** argv)
{
#if LE_PLATFORM_WINDOWS
	SetUnhandledExceptionFilter(ExceptionHandler);
#endif
	
	int Result = 0;
	{
		Lumina::FApplicationGlobalState GlobalState;
		Lumina::FApplication* App = Lumina::CreateApplication(argc, argv);
		Result = App->Run(argc, argv);
	}
	
	
	return Result;
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

