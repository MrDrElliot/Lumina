#pragma once

#include "Layer.h"
#include <vector>

namespace Lumina
{
	class FLayerStack
	{
	public:

		FLayerStack();
		virtual ~FLayerStack();

		void PushLayer(FLayer* InLayer);
		void PushOverlay(FLayer* InLayer);
		void PopLayer(FLayer* InLayer);
		void PopOverlay(FLayer* InLayer);

		FLayer* operator [](size_t i)
		{
			return Layers[i];
		}

		const FLayer* operator [](size_t i) const
		{
			return Layers[i];
		}

		size_t GetSize() const { return Layers.size(); }

		std::vector<FLayer*>::iterator begin() { return Layers.begin(); }
		std::vector<FLayer*>::iterator end() { return Layers.end(); }

	private:

		std::vector<FLayer*> Layers;
		unsigned int LayerInsertIndex = 0;
	};
}