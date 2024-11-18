#pragma once

#include <Core/Serialization/Archiver.h>

namespace Lumina
{
	class FComponent
	{
	public:

		FComponent() =			default;
		virtual ~FComponent() = default;
		
		virtual void Serialize(FArchive& Ar) = 0;
	
	protected:

		LString DisplayName;

	};
}