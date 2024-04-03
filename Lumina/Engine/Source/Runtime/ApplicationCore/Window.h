#pragma once

#include <cstdint>
#include <string>

namespace Lumina
{

	struct FWindowSpecs
	{
		std::string Title = "Lumina";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class FWindow
	{
	public:
		static FWindow* Create(const FWindowSpecs& InSpecs);

		FWindow(const FWindowSpecs& InSpecs, bool bInit = false);
		virtual ~FWindow();


		virtual void Init();


	private:

		bool bInitialized;
		FWindowSpecs Specs;
	};
}