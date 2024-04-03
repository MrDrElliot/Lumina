#include "LayerStack.h"

Lumina::FLayerStack::FLayerStack()
{
}

Lumina::FLayerStack::~FLayerStack()
{
}

void Lumina::FLayerStack::PushLayer(FLayer* InLayer)
{
	Layers.emplace(Layers.begin() + LayerInsertIndex, InLayer);
	LayerInsertIndex++;
}

void Lumina::FLayerStack::PushOverlay(FLayer* InLayer)
{
	Layers.emplace_back(InLayer);
}

void Lumina::FLayerStack::PopLayer(FLayer* InLayer)
{
	auto it = std::find(Layers.begin(), Layers.end(), InLayer);
	if (it != Layers.end())
	{
		Layers.erase(it);
		LayerInsertIndex--;
	}
}

void Lumina::FLayerStack::PopOverlay(FLayer* InLayer)
{
	auto it = std::find(Layers.begin(), Layers.end(), InLayer);
	if (it != Layers.end())
	{
		Layers.erase(it);
	}
}
