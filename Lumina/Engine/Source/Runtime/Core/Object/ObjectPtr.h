#pragma once
#include <atomic>

#include "Core/Assertions/Assert.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
	class CObject;
	
	template<typename ReferencedType>
	class TObjectPtr
	{
		typedef ReferencedType* ReferenceType;

	public:

		FORCEINLINE TObjectPtr():
			Reference(nullptr)
		{ }

		TObjectPtr(ReferencedType* InReference, bool bAddRef = true)
		{
			Reference = InReference;
			if(Reference && bAddRef)
			{
				Reference->AddRef();
			}
		}

		TObjectPtr(const TObjectPtr& Copy)
		{
			Reference = Copy.Reference;
			if(Reference)
			{
				Reference->AddRef();
			}
		}

		template<typename CopyReferencedType>
		requires std::is_base_of_v<ReferencedType, CopyReferencedType>
		TObjectPtr(const TObjectPtr<CopyReferencedType>& Copy)
		{
			Reference = static_cast<ReferencedType*>(Copy.GetReference());
			if (Reference)
			{
				Reference->AddRef();
			}
		}

		FORCEINLINE TObjectPtr(TObjectPtr&& Move) noexcept
		{
			Reference = Move.Reference;
			Move.Reference = nullptr;
		}

		template<typename MoveReferencedType>
		requires std::is_base_of_v<ReferencedType, MoveReferencedType>
		TObjectPtr(TObjectPtr<MoveReferencedType>&& Move)
		{
			Reference = static_cast<ReferencedType*>(Move.GetReference());
			Move.Reference = nullptr;
		}


		~TObjectPtr()
		{
			if(Reference)
			{
				Reference->Release();
			}
		}

		TObjectPtr& operator=(ReferencedType* InReference)
		{
			if (Reference != InReference)
			{
				// Call AddRef before Release, in case the new reference is the same as the old reference.
				ReferencedType* OldReference = Reference;
				Reference = InReference;
				if (Reference)
				{
					Reference->AddRef();
				}
				if (OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		template<typename T>
		TObjectPtr<T> As()
		{
			Assert(GetRefCount() > 0);
			return TRefCountPtr<T>(static_cast<T*>(Reference));
		}

		template<typename T>
		const TObjectPtr<T> As() const
		{
			Assert(GetRefCount() > 0);
			return TRefCountPtr<T>(static_cast<T*>(Reference));
		}
	
		FORCEINLINE TObjectPtr& operator=(const TObjectPtr& InPtr)
		{
			return *this = InPtr.Reference;
		}

		template<typename CopyReferencedType>
		FORCEINLINE TObjectPtr& operator=(const TObjectPtr<CopyReferencedType>& InPtr)
		{
			return *this = InPtr.GetReference();
		}

		TObjectPtr& operator=(TObjectPtr&& InPtr) noexcept
		{
			if (this != &InPtr)
			{
				ReferencedType* OldReference = Reference;
				Reference = InPtr.Reference;
				InPtr.Reference = nullptr;
				if(OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		template<typename MoveReferencedType>
		TObjectPtr& operator=(TObjectPtr<MoveReferencedType>&& InPtr)
		{
			// InPtr is a different type (or we would have called the other operator), so we need not test &InPtr != this
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = nullptr;
			if (OldReference)
			{
				OldReference->Release();
			}
			return *this;
		}

		FORCEINLINE ReferencedType* operator->() const
		{
			return Reference;
		}

		FORCEINLINE operator ReferenceType() const
		{
			return Reference;
		}

		FORCEINLINE ReferencedType** GetInitReference()
		{
			*this = nullptr;
			return &Reference;
		}

		FORCEINLINE ReferencedType* GetReference() const
		{
			return Reference;
		}

		FORCEINLINE friend bool IsValidRef(const TObjectPtr& InReference)
		{
			return InReference.Reference != nullptr;
		}

		FORCEINLINE bool IsValid() const
		{
			return Reference != nullptr;
		}

		FORCEINLINE void SafeRelease()
		{
			*this = nullptr;
		}

		uint32 GetRefCount() const
		{
			uint32 Result = 0;
			if (Reference)
			{
				Result = Reference->GetRefCount();
				Assert(Result > 0);
			}
			return Result;
		}

		FORCEINLINE void Swap(TObjectPtr& InPtr)
		{
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = OldReference;
		}

	private:

		ReferencedType* Reference;

		template <typename OtherType>
		friend class TObjectPtr;

	public:
		FORCEINLINE bool operator==(const TObjectPtr& B) const
		{
			return GetReference() == B.GetReference();
		}

		FORCEINLINE bool operator==(ReferencedType* B) const
		{
			return GetReference() == B;
		}
	};

}
