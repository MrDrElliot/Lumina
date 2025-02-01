#pragma once


namespace Lumina
{
	class FEntityComponent
	{
	public:

		FEntityComponent() =			default;
		virtual ~FEntityComponent() =	default;
	
	protected:

		FString DisplayName;

	};
}