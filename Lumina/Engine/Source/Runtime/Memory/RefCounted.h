#pragma once

#include "Core/Templates/Forward.h"
#include "Memory.h"
#include "Core/Assertions/Assert.h"
#include "Core/Object/Object.h"
#include "Log/Log.h"

namespace Lumina
{
	/** A virtual interface for ref counted objects to implement. */
	class IRefCountedObject
	{
	public:
		virtual ~IRefCountedObject() { }
		virtual uint32 AddRef() const = 0;
		virtual uint32 Release() const = 0;
		virtual uint32 GetRefCount() const = 0;
	};

	/** Intrusive Smart Pointer Implementation */
	class FRefCounted : public LEObject
	{
	public:

		FRefCounted() :RefCount(0) {}
		virtual ~FRefCounted() { Assert(RefCount == 0); }
		FRefCounted(const FRefCounted&) = delete;
		FRefCounted& operator = (const FRefCounted&) = delete;

		FORCEINLINE void AddRef() const
		{
			/** Add 1 to the reference count */
			RefCount.fetch_add(1, eastl::memory_order_relaxed);
		}

		FORCEINLINE void Release() const
		{
			/** Returns the previous value (if previous value is 1, our new value is 0). */
			if(RefCount.fetch_sub(1, eastl::memory_order_acq_rel) == 1)
			{
				eastl::atomic_thread_fence(eastl::memory_order_acquire);
				FRefCounted* MutableThis = const_cast<FRefCounted*>(this);
				FMemory::Delete(MutableThis);
			}
		}

		FORCEINLINE uint32 GetRefCount() const { return RefCount; }

		FORCEINLINE bool IsValid() const { return RefCount > 0; }

	private:

		/** Atomic for safety across threads */
		mutable eastl::atomic<int> RefCount = 0;

	};

	/**
	 * A smart pointer to an object which implements AddRef/Release.
	 */
	template<typename ReferencedType>
	class TRefCountPtr
	{
		typedef ReferencedType* ReferenceType;

	public:

		FORCEINLINE TRefCountPtr():
			Reference(nullptr)
		{ }

		TRefCountPtr(ReferencedType* InReference, bool bAddRef = true)
		{
			Reference = InReference;
			if(Reference && bAddRef)
			{
				Reference->AddRef();
			}
		}

		TRefCountPtr(const TRefCountPtr& Copy)
		{
			Reference = Copy.Reference;
			if(Reference)
			{
				Reference->AddRef();
			}
		}

		template<typename CopyReferencedType>
		requires std::is_base_of_v<ReferencedType, CopyReferencedType>
		TRefCountPtr(const TRefCountPtr<CopyReferencedType>& Copy)
		{
			Reference = static_cast<ReferencedType*>(Copy.GetReference());
			if (Reference)
			{
				Reference->AddRef();
			}
		}

		FORCEINLINE TRefCountPtr(TRefCountPtr&& Move) noexcept
		{
			Reference = Move.Reference;
			Move.Reference = nullptr;
		}

		template<typename MoveReferencedType>
		requires std::is_base_of_v<ReferencedType, MoveReferencedType>
		TRefCountPtr(TRefCountPtr<MoveReferencedType>&& Move)
		{
			Reference = static_cast<ReferencedType*>(Move.GetReference());
			Move.Reference = nullptr;
		}


		~TRefCountPtr()
		{
			if(Reference)
			{
				Reference->Release();
			}
		}

		TRefCountPtr& operator=(ReferencedType* InReference)
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
		TRefCountPtr<T> As()
		{
			Assert(GetRefCount() > 0);
			return TRefCountPtr<T>(static_cast<T*>(Reference));
		}

		template<typename T>
		const TRefCountPtr<T> As() const
		{
			Assert(GetRefCount() > 0);
			return TRefCountPtr<T>(static_cast<T*>(Reference));
		}
	
		FORCEINLINE TRefCountPtr& operator=(const TRefCountPtr& InPtr)
		{
			return *this = InPtr.Reference;
		}

		template<typename CopyReferencedType>
		FORCEINLINE TRefCountPtr& operator=(const TRefCountPtr<CopyReferencedType>& InPtr)
		{
			return *this = InPtr.GetReference();
		}

		TRefCountPtr& operator=(TRefCountPtr&& InPtr) noexcept
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
		TRefCountPtr& operator=(TRefCountPtr<MoveReferencedType>&& InPtr)
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

		FORCEINLINE friend bool IsValidRef(const TRefCountPtr& InReference)
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
				Assert(Result > 0); // you should never have a zero ref count if there is a live ref counted pointer (*this is live)
			}
			return Result;
		}

		FORCEINLINE void Swap(TRefCountPtr& InPtr) // this does not change the reference count, and so is faster
		{
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = OldReference;
		}

	private:

		ReferencedType* Reference;

		template <typename OtherType>
		friend class TRefCountPtr;

	public:
		FORCEINLINE bool operator==(const TRefCountPtr& B) const
		{
			return GetReference() == B.GetReference();
		}

		FORCEINLINE bool operator==(ReferencedType* B) const
		{
			return GetReference() == B;
		}
	};
	

	
}

namespace eastl
{
	template <typename T>
	struct hash<Lumina::TRefCountPtr<T>>
	{
		std::size_t operator()(const Lumina::TRefCountPtr<T>& handle) const noexcept
		{
			return eastl::hash<T*>()(handle.GetReference());
		}
	};
}

template<typename T, typename... TArgs>
requires std::is_constructible_v<T, TArgs...> && (!eastl::is_array_v<T>)
FORCEINLINE Lumina::TRefCountPtr<T> MakeRefCount(TArgs&&... Args)
{
	return Lumina::TRefCountPtr<T>(FMemory::New<T>(TForward<TArgs>(Args)...));
}
