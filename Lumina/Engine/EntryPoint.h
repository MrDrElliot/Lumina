#pragma once

#include "Source/Runtime/ApplicationCore/Application.h"

extern std::unique_ptr<Lumina::FApplication> Lumina::CreateApplication(int argc, char** argv);

inline bool gApplicationRunning = true;

namespace Lumina
{
	inline int GuardedMain(int argc, char** argv)
	{
		int Exit = 0;
		while (gApplicationRunning)
		{
			std::unique_ptr<FApplication> App = CreateApplication(argc, argv);
			App->Run();
			Exit = App->Close();
	
			gApplicationRunning = false;
		}
		return Exit;
	}

}

#if LE_PLATFORM_WINDOWS
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Lumina::GuardedMain(__argc, __argv);
}
#else

inline int main(int argc, char** argv)
{
	return Lumina::GuardedMain(argc, argv);
}

#endif