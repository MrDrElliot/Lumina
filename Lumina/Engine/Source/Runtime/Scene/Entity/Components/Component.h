#pragma once

#include "Core/Object/ObjectMacros.h"
#include "EntityComponentRegistry.h"
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

#define ENTITY_COMPONENT(Type) \
    virtual Lumina::CStruct* GetType() override { return StaticStruct(); }
}

	
