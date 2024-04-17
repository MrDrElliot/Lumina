#include "LayerStack.h"

Lumina::FLayerStack::FLayerStack()
{
}

Lumina::FLayerStack::~FLayerStack()
{
}

void Lumina::FLayerStack::PushLayer(std::shared_ptr<FLayer> InLayer)
{
	Layers.emplace(Layers.begin() + LayerInsertIndex, InLayer);
	LayerInsertIndex++;
}

void Lumina::FLayerStack::PushOverlay(std::shared_ptr<FLayer> InLayer)
{
	Layers.emplace_back(InLayer);
}

void Lumina::FLayerStack::PopLayer(std::shared_ptr<FLayer> InLayer)
{
	auto it = std::find(Layers.begin(), Layers.end(), InLayer);
	if (it != Layers.end())
	{
		Layers.erase(it);
		LayerInsertIndex--;
	}
}

void Lumina::FLayerStack::PopOverlay(std::shared_ptr<FLayer> InLayer)
{
	auto it = std::find(Layers.begin(), Layers.end(), InLayer);
	if (it != Layers.end())
	{
		Layers.erase(it);
	}
}
