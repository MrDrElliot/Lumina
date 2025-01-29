#pragma once

#include "Memory/RefCounted.h"
#include "Events/Event.h"

namespace Lumina
{
	class FLayer : public FRefCounted
	{
	public:

		FLayer() = default;
		
		FLayer(const FString& InName)
		{
			DebugName = InName;
		}

		virtual ~FLayer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double DeltaTime) {}
		virtual void OnEvent(FEvent& InEvent) {}
		virtual void ImGuiRender(double DeltaTime) {}

		inline const FString& GetName() const { return DebugName; }

	private:

		FString DebugName;
	};
}
