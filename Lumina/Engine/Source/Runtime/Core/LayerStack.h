#pragma once

#include <typeindex>
#include "Containers/Array.h"
#include "Memory/RefCounted.h"
#include <Platform/GenericPlatform.h>
#include "Layer.h"


namespace Lumina
{

	class FLayerStack
	{
	public:
		
		friend class FLayer;

		FLayerStack();
		virtual ~FLayerStack();

		void PushLayer(const TRefPtr<FLayer>& InLayer);
		void PushOverlay(const TRefPtr<FLayer>& InLayer);
		void PopLayer(const TRefPtr<FLayer>& InLayer);
		void PopOverlay(const TRefPtr<FLayer>& InLayer);
		void DetachAllLayers();

		TRefPtr<FLayer> operator [](uint32 i)
		{
			return Layers[i];
		}

		uint32 GetSize() const { return Layers.size(); }

		TArray<TRefPtr<FLayer>>::iterator begin() { return Layers.begin(); }
		TArray<TRefPtr<FLayer>>::iterator end() { return Layers.end(); }


		template<typename T>
		TRefPtr<T> GetLayerByType()
		{
			auto it = LayerMap.find(typeid(T));
			if(it != LayerMap.end())
			{
				return RefPtrCast<T>(it->second);
			}
			
			return TRefPtr<T>();
		}

	private:

		TArray<TRefPtr<FLayer>> Layers;
		std::unordered_map<std::type_index, TRefPtr<FLayer>> LayerMap;
		
		uint32 LayerInsertIndex = 0;
	};
}
