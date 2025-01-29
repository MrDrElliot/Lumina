#pragma once
#include <Assets/AssetTypes.h>
#include <functional>

namespace Lumina
{
	class FAssetPropertyPanel
	{
		
	public:

		static bool Render(EAssetType Type, FAssetMetadata& Selected);
	};
}