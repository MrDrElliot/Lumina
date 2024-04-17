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

		void PushLayer(std::shared_ptr<FLayer> InLayer);
		void PushOverlay(std::shared_ptr<FLayer> InLayer);
		void PopLayer(std::shared_ptr<FLayer> InLayer);
		void PopOverlay(std::shared_ptr<FLayer> InLayer);

		std::shared_ptr<FLayer> operator [](size_t i)
		{
			return Layers[i];
		}

		const std::shared_ptr<FLayer> operator [](size_t i) const
		{
			return Layers[i];
		}

		size_t GetSize() const { return Layers.size(); }

		std::vector<std::shared_ptr<FLayer>>::iterator begin() { return Layers.begin(); }
		std::vector<std::shared_ptr<FLayer>>::iterator end() { return Layers.end(); }

	private:

		std::vector<std::shared_ptr<FLayer>> Layers;
		unsigned int LayerInsertIndex = 0;
	};
}