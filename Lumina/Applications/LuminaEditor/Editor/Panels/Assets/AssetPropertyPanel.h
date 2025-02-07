#pragma once


namespace Lumina
{
	enum class EAssetType : unsigned char;
	struct FAssetHeader;
	
	class FAssetPropertyPanel
	{
		
	public:

		static bool Render(EAssetType Type, FAssetHeader& Selected);
	};
}
