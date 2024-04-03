#pragma once

#include <string>

namespace Lumina
{
	class FLayer
	{
	public:
		
		FLayer(const std::string& InName = "Layer")
		{
			DebugName = InName;
		}

		virtual ~FLayer()
		{

		}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float DeltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent() {}

		inline const std::string& GetName() const { return DebugName; }

	private:

		std::string DebugName;
	};
}