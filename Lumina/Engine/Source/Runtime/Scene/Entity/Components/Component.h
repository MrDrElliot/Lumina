#pragma once

#include "ComponentRegistry.h"
#include "Core/Object/ObjectMacros.h"
#include "Component.generated.h"

namespace Lumina
{
	LUM_STRUCT()
	struct LUMINA_API SEntityComponent
	{
		GENERATED_BODY()

		virtual CStruct* GetType()
		{
			return nullptr;
		}
	};

#define ENTITY_COMPONENT() \
	virtual Lumina::CStruct* GetType() override { return StaticStruct(); }
}
