#include "Window.h"


namespace Lumina
{
	FWindow::FWindow(const FWindowSpecs& InSpecs, bool bInit)
	{
		Specs = InSpecs;

		if (bInit)
		{
			Init();
		}
	}

	FWindow::~FWindow()
	{
	}

	void FWindow::Init()
	{
		if (!bInitialized)
		{

		}
	}

	FWindow* FWindow::Create(const FWindowSpecs& InSpecs)
	{
		return new FWindow(InSpecs);
	}
}