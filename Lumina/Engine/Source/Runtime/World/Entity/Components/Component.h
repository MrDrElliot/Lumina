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

	protected:
		
		
	};

	
#define ENTITY_COMPONENT() \
    virtual Lumina::CStruct* GetType() override { return StaticStruct(); }
}

	
