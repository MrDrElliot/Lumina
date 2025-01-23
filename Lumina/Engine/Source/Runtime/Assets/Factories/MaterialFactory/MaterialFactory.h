#pragma once

#include "Assets/Factories/Factory.h"

namespace Lumina
{
	class FMaterialFactory : public FFactory
	{
	public:

		virtual std::shared_ptr<LAsset> CreateNew(const FAssetMetadata& Metadata, FArchive& Archive) override;

	};
}