#pragma once

#include "Memory/RefCounted.h"
#include "Containers/String.h"
#include "Events/Event.h"

namespace Lumina
{
	class FLayer : public RefCounted
	{
	public:

		FLayer() = default;
		
		FLayer(const std::string& InName)
		{
			DebugName = InName;
		}

		virtual ~FLayer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double DeltaTime) {}
		virtual void OnEvent(FEvent& InEvent) {}
		virtual void ImGuiRender(double DeltaTime) {}

		inline const LString& GetName() const { return DebugName; }

	private:

		LString DebugName;
	};
}
