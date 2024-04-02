#pragma once
#include "Source/Runtime/Application.h"

extern Lumina::FApplication* Lumina::CreateApplication(int argc, char** argv);

inline bool gApplicationRunning = true;

namespace Lumina
{
	int GuardedMain(int argc, char** argv)
	{
		while (gApplicationRunning)
		{
			FApplication* App = CreateApplication(argc, argv);
			App->Run();
			delete App;
		}
		return 0;
	}

}

#if LE_PLATFORM_WINDOWS
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Lumina::GuardedMain(__argc, __argv);
}
#else

int main(int argc, char** argv)
{
	return Lumina::GuardedMain(argc, argv);
}

#endif