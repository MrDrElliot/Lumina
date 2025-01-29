
#include "LayerStack.h"
#include "Layer.h"


namespace Lumina
{
	FLayerStack::FLayerStack()
	{
	}

	FLayerStack::~FLayerStack()
	{
	}

	void FLayerStack::PushLayer(const TRefPtr<FLayer>& InLayer)
	{
		Layers.emplace_back(InLayer);
		LayerMap[typeid(InLayer)] = InLayer;
		LayerInsertIndex++;
	}

	void FLayerStack::PushOverlay(const TRefPtr<FLayer>& InLayer)
	{
		Layers.push_back(InLayer);
	}

	void FLayerStack::PopLayer(const TRefPtr<FLayer>& InLayer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), InLayer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			auto mapit = LayerMap.find(typeid(InLayer));
			if(mapit != LayerMap.end())
			{
				LayerMap.erase(mapit);
			}
			LayerInsertIndex--;
		}
	}

	void FLayerStack::PopOverlay(const TRefPtr<FLayer>& InLayer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), InLayer);
		if (it != Layers.end())
		{
			Layers.erase(it);
		}
	}

	void FLayerStack::DetachAllLayers()
	{
		for (auto Layer : Layers)
		{
			Layer->OnDetach();
		}

		Layers.clear();
	}
}
