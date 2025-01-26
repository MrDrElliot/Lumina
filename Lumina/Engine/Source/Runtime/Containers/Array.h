#pragma once

#include <iostream>
#include "Core/Assertions/Assert.h"
#include "Core/Templates/CanBulkSerialize.h"
#include "Core/Serialization/Archiver.h"

#define TArrayMaxSize UINT32_MAX
#pragma warning(push)
#pragma warning(disable :4244)
#pragma warning(disable: 4267)


namespace Lumina
{
	template <typename T>
	class TArray
	{
	public:
		// types:
		typedef T                                     ValueType;
		typedef T &                                   reference;
		typedef const T &                             const_reference;
		typedef T *                                   pointer;
		typedef const T *                             const_pointer;
		typedef T *                                   iterator;
		typedef const T *                             const_iterator;
		typedef std::reverse_iterator<iterator>       reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		typedef ptrdiff_t                             difference_type;
		typedef unsigned int                          SizeType;

		// 23.3.11.2, construct/copy/destroy:
		TArray() noexcept;
		explicit TArray(SizeType n);
		TArray(SizeType n, const T &val);
		template <class InputIt> TArray(InputIt first, InputIt last); //v1(v2.begin(),v2.end())
		TArray(std::initializer_list<T>);
		TArray(const TArray<T> &);
		TArray(TArray<T> &&) noexcept;
		~TArray();
		TArray<T> & operator = (const TArray<T> &);
		TArray<T> & operator = (TArray<T> &&) noexcept;
		TArray<T> & operator = (std::initializer_list<T>);
		void assign(SizeType, const T &value);
		template <class InputIt> void assign(InputIt, InputIt);
		void assign(std::initializer_list<T>);

		// iterators:
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		reverse_iterator rbegin() noexcept;
		const_reverse_iterator crbegin() const noexcept;
		reverse_iterator rend() noexcept;
		const_reverse_iterator crend() const noexcept;

		// 23.3.11.3, capacity:
		bool empty() const noexcept;
		SizeType size() const noexcept;
		SizeType max_size() const noexcept;
		SizeType capacity() const noexcept;
		void resize(SizeType);
		void resize(SizeType, const T &);
		void reserve(SizeType);
		void ShrinkToFit();

		// element access
		reference operator [](SizeType);
		const_reference operator [](SizeType) const;
		reference at(SizeType);
		const_reference at(SizeType) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;

		// 23.3.11.4, data access:
		T * data() noexcept;
		const T * data() const noexcept; 

		// 23.3.11.5, modifiers:
		template <class ... Args> void EmplaceBack(Args && ... args);
		template <class ... Args> ValueType& EmplaceBack_GetRef(Args&&... args);
		void PushBack(const T &);
		void PushBack(T &&);
		void PopBack();
		
		template <class ... Args> iterator Emplace(const_iterator, Args && ...);
		
		iterator insert(const_iterator, const T &);
		iterator insert(const_iterator, T &&);
		iterator insert(const_iterator, SizeType, const T&);
		template <class InputIt> iterator insert(const_iterator, InputIt, InputIt);
		iterator insert(const_iterator, std::initializer_list<T>);
		iterator erase(const_iterator);
		iterator erase(const_iterator, const_iterator);
		void swap(TArray<T> &) noexcept;
		void clear() noexcept;
		SizeType AddUnitialized(SizeType Count);

		bool operator == (const TArray<T> &) const;
		bool operator != (const TArray<T> &) const;
		bool operator < (const TArray<T> &) const;
		bool operator <= (const TArray<T> &) const;
		bool operator > (const TArray<T> &) const;
		bool operator >= (const TArray<T> &) const;
			
		friend void Print(const TArray<T> &, std::string);
		
	private:
		
		SizeType ReserveSize = 4;
		SizeType VectorSize = 0;
		T *ptr;

		inline void reallocate();
		

		friend FArchive& operator << (FArchive& Ar, TArray<ValueType>& Array)
		{
			SizeType SerializeNum = Ar.IsReading() ? 0 : Array.size();
			
			Ar << SerializeNum;

			if (SerializeNum == 0)
			{
				if (Ar.IsReading())
				{
					Array.clear();
				}

				return Ar;
			}

			if (Ar.HasError() || SerializeNum < 0)
			{
				Ar.SetHasError(true);
				return Ar;
			}

			// Case for bulk serialization (e.g., small types like uint8 or types that support it)
			if constexpr (sizeof(ValueType) == 1 || TCanBulkSerialize<ValueType>::Value)
			{
				if (Ar.IsReading())
				{
					Array.resize(SerializeNum); // Resize the array based on serialized number
				}

				Ar.Serialize(Array.data(), Array.size() * sizeof(ValueType)); // Bulk serialize data
			}
			else
			{
				if (Ar.IsReading())
				{
					for (SizeType i = 0; i < SerializeNum; i++)
					{
						Array.EmplaceBack();
						Ar << Array.back();
					}
				}
				else
				{
					for (SizeType i = 0; i < SerializeNum; i++)
					{
						Ar << Array[i];
					}
				}
			}

			return Ar;
		}
	};



	
	template <typename T>
	TArray<T>::TArray() noexcept
	{
		ptr = new T[ReserveSize];
	}

	template <typename T>
	TArray<T>::TArray(TArray<T>::SizeType n)
	{
		SizeType i;
		ReserveSize = n << 2;
		ptr = new T[ReserveSize];
		for (i = 0; i < n; ++i)
		{
			ptr[i] = T();
		}
		VectorSize = n;
	}

	template <typename T>
	TArray<T>::TArray(typename TArray<T>::SizeType n, const T &value)
	{
		SizeType i;
		ReserveSize = n << 2;
		ptr = new T[ReserveSize];
		for (i = 0; i < n; ++i)
		{
			ptr[i] = value;
		}
		VectorSize = n;
	}

	template <typename T>
	template <class InputIt>
	TArray<T>::TArray(InputIt first, InputIt last)
	{
		SizeType i, count = last - first;
		ReserveSize = count << 2;
		VectorSize = count;
		ptr = new T[ReserveSize];
		for (i = 0; i < count; ++i, ++first)
		{
			ptr[i] = *first;
		}
	}

	template <typename T>
	TArray<T>::TArray(std::initializer_list<T> lst)
	{
		ReserveSize = lst.size() << 2;
		ptr = new T[ReserveSize];
		for (auto &item: lst)
		{
			ptr[VectorSize++] = item;
		}
	}

	template <typename T>
	TArray<T>::TArray(const TArray<T> &other)
	{
		SizeType i;
		ReserveSize = other.ReserveSize;
		ptr = new T[ReserveSize];
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = other.ptr[i];
		}
		VectorSize = other.VectorSize;
	}

	template <typename T>
	TArray<T>::TArray(TArray<T> &&other) noexcept
	{
		SizeType i;
		ReserveSize = other.ReserveSize;
		ptr = new T[ReserveSize];
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = std::move(other.ptr[i]);
		}
		VectorSize = other.VectorSize;
	}

	template <typename T>
	TArray<T>::~TArray()
	{
		delete [] ptr;
		ptr = nullptr;
	}

	template <typename T>
	TArray<T> & TArray<T>::operator = (const TArray<T> &other)
	{
		if(this == &other)
		{
			return *this;
		}
		SizeType i;
		if (ReserveSize < other.VectorSize)
		{
			ReserveSize = other.VectorSize << 2;
			reallocate();
		}
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = other.ptr[i];
		}
		VectorSize = other.VectorSize;
		return *this;
	}

	template <typename T>
	TArray<T>& TArray<T>::operator = (TArray<T> &&other) noexcept
	{
		SizeType i;
		if (ReserveSize < other.VectorSize)
		{
			ReserveSize = other.VectorSize << 2;
			reallocate();
		}
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = std::move(other.ptr[i]);
		}
		VectorSize = other.VectorSize;
		return *this;
	}

	template <typename T>
	TArray<T> & TArray<T>::operator = (std::initializer_list<T> lst) {
		if (ReserveSize < lst.size())
		{
			ReserveSize = lst.size() << 2;
			reallocate();
		}
		VectorSize = 0;
		for (auto &item: lst)
		{
			ptr[VectorSize++] = item;
		}
		return *this;
	}

	template <typename T>
	void TArray<T>::assign(typename TArray<T>::SizeType count, const T &value) {
		SizeType i;
		if (count > ReserveSize) {
			ReserveSize = count << 2;
			reallocate();
		}
		for (i = 0; i < count; ++i)
			ptr[i] = value;
		VectorSize = count;
	}

	template <typename T>
	template <class InputIt>
	void TArray<T>::assign(InputIt first, InputIt last) {
		SizeType i, count = last - first;
		if (count > ReserveSize) {
			ReserveSize = count << 2;
			reallocate();
		}
		for (i = 0; i < count; ++i, ++first)
			ptr[i] = *first;
		VectorSize = count;
	}

	template <typename T>
	void TArray<T>::assign(std::initializer_list<T> lst) {
		SizeType i, count = lst.size();
		if (count > ReserveSize) {
			ReserveSize = count << 2;
			reallocate();
		}
		i = 0;
		for (auto &item: lst)
			ptr[i++] = item;
	}


	template <typename T>
	typename TArray<T>::iterator TArray<T>::begin() noexcept {
		return ptr;
	}

	template <typename T>
	typename TArray<T>::const_iterator TArray<T>::begin() const noexcept {
		return ptr;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::end() noexcept {
		return ptr + VectorSize;
	}

	template <typename T>
	typename TArray<T>::const_iterator TArray<T>::end() const noexcept {
		return ptr + VectorSize;
	}

	template <typename T>
	typename TArray<T>::reverse_iterator TArray<T>::rbegin() noexcept {
		return reverse_iterator(ptr + VectorSize);
	}

	template <typename T>
	typename TArray<T>::const_reverse_iterator TArray<T>::crbegin() const noexcept {
		return reverse_iterator(ptr + VectorSize);
	}

	template <typename T>
	typename TArray<T>::reverse_iterator TArray<T>::rend() noexcept {
		return reverse_iterator(ptr);
	}
	
	template <typename T>
	typename TArray<T>::const_reverse_iterator TArray<T>::crend() const noexcept {
		return reverse_iterator(ptr);
	}

	template <typename T>
	void TArray<T>::reallocate()
	{
		AssertMsg(VectorSize <= ReserveSize, "VectorSize must not exceed ReserveSize.");

		// Check for potential overflow
		if (VectorSize > (INT32_MAX / sizeof(T)))
		{
			// Log the actual potential overflow size
			int64 totalSize = static_cast<int64>(VectorSize) * sizeof(T);
			LOG_CRITICAL("VectorSize * sizeof({0}) would overflow. Total size: {1}", typeid(T).name(), totalSize);
		}


		T* tptr = new T[ReserveSize];

		if constexpr (std::is_trivially_copyable<T>::value)
		{
			memcpy(tptr, ptr, VectorSize * sizeof(T));
		}
		else
		{
			for (SizeType i = 0; i < VectorSize; ++i)
			{
				new (&tptr[i]) T(std::move(ptr[i]));
				ptr[i].~T();
			}
		}

		delete[] ptr;
		ptr = tptr;
	}


	template <typename T>
	bool TArray<T>::empty() const noexcept {
		return VectorSize == 0;
	}
	
	template <typename T>
	typename TArray<T>::SizeType TArray<T>::size() const noexcept
	{
		return VectorSize;
	}

	template <typename T>
	typename TArray<T>::SizeType TArray<T>::max_size() const noexcept
	{
		return TArrayMaxSize;
	}

	template <typename T>
	typename TArray<T>::SizeType TArray<T>::capacity() const noexcept
	{
		return ReserveSize;
	}
	
	template <typename T>
	void TArray<T>::resize(typename TArray<T>::SizeType sz)
	{
		if (sz > VectorSize)
		{
			if (sz > ReserveSize)
			{
				ReserveSize = sz;
				reallocate();
			}
		}
		else
		{
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
			{
				ptr[i].~T();
			}
		}
		VectorSize = sz;
	}

	template <typename T>
	void TArray<T>::resize(typename TArray<T>::SizeType sz, const T &c) {
		if (sz > VectorSize)
		{
			if (sz > ReserveSize)
			{
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		} else
		{
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i].~T();
		}
		VectorSize = sz;
	}

	template <typename T>
	void TArray<T>::reserve(TArray<T>::SizeType _sz)
	{
		if (_sz > ReserveSize)
		{
			ReserveSize = _sz;
			reallocate();
		}
	}

	template <typename T>
	void TArray<T>::ShrinkToFit()
	{
		ReserveSize = VectorSize;
		reallocate();
	}

	
	template <typename T>
	typename TArray<T>::reference TArray<T>::operator [](TArray<T>::SizeType idx) {
		return ptr[idx];
	}

	template <typename T>
	typename TArray<T>::const_reference TArray<T>::operator [](TArray<T>::SizeType idx) const {
		return ptr[idx];
	}

	template <typename T>
	typename TArray<T>::reference TArray<T>::at(SizeType pos) {
		if (pos < VectorSize)
			return ptr[pos];
		else
			throw std::out_of_range("accessed position is out of range");
	}

	template <typename T>
	typename TArray<T>::const_reference TArray<T>::at(SizeType pos) const {
		if (pos < VectorSize)
			return ptr[pos];
		else
			throw std::out_of_range("accessed position is out of range");
	}

	template <typename T>
	typename TArray<T>::reference TArray<T>::front() {
		return ptr[0];
	}

	template <typename T>
	typename TArray<T>::const_reference TArray<T>::front() const {
		return ptr[0];
	}

	template <typename T>
	typename TArray<T>::reference TArray<T>::back() {
		return ptr[VectorSize - 1];
	}

	template <typename T>
	typename TArray<T>::const_reference TArray<T>::back() const
	{
		return ptr[VectorSize - 1];
	}

	
	template <typename T>
	T* TArray<T>::data() noexcept
	{
		return ptr;
	}

	template <typename T>
	const T* TArray<T>::data() const noexcept
	{
		return ptr;
	}


	template <typename T>
	template <class ... Args>
	void TArray<T>::EmplaceBack(Args && ... args)
	{
		if (VectorSize == ReserveSize)
		{
			ReserveSize <<= 2;
			reallocate();
		}
		ptr[VectorSize] = std::move( T( std::forward<Args>(args) ... ) );
		++VectorSize;
	}

	template <typename T>
	template <class... Args>
	typename TArray<T>::ValueType& TArray<T>::EmplaceBack_GetRef(Args&&... args)
	{
		if (VectorSize == ReserveSize)
		{
			ReserveSize = (ReserveSize > 0) ? ReserveSize * 2 : 1;
			reallocate();
		}

		// Initialize a new object in the next available slot
		ptr[VectorSize] = T(std::forward<Args>(args)...);
		return ptr[VectorSize++];
	}



	template <typename T>
	void TArray<T>::PushBack(const T &val) {
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		ptr[VectorSize] = val;
		++VectorSize;
	}

	template <typename T>
	void TArray<T>::PushBack(T &&val) {
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		ptr[VectorSize] = std::move(val);
		++VectorSize;
	}

	template <typename T>
	void TArray<T>::PopBack() {
		--VectorSize;
		ptr[VectorSize].~T();
	}


	template <typename T>
	template <class ... Args>
	typename TArray<T>::iterator TArray<T>::Emplace(typename TArray<T>::const_iterator it, Args && ... args) {
		iterator iit = &ptr[it - ptr];
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		memmove(iit + 1, iit, (VectorSize - (it - ptr)) * sizeof(T));
		(*iit) = std::move( T( std::forward<Args>(args) ... ) );
		++VectorSize;
		return iit;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::insert(typename TArray<T>::const_iterator it, const T &val) {
		iterator iit = &ptr[it - ptr];
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		memmove(iit + 1, iit, (VectorSize - (it - ptr)) * sizeof(T));
		(*iit) = val;
		++VectorSize;
		return iit;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::insert(typename TArray<T>::const_iterator it, T &&val)
	{
		iterator iit = &ptr[it - ptr];
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		memmove(iit + 1, iit, (VectorSize - (it - ptr)) * sizeof(T));
		(*iit) = std::move(val);
		++VectorSize;
		return iit;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::insert(TArray<T>::const_iterator it, TArray<T>::SizeType cnt, const T &val) {
		iterator f = &ptr[it - ptr];
		if (!cnt) return f;
		if (VectorSize + cnt > ReserveSize) {
			ReserveSize = (VectorSize + cnt) << 2;
			reallocate();
		}
		memmove(f + cnt, f, (VectorSize - (it - ptr)) * sizeof(T));
		VectorSize += cnt;
		for (iterator it = f; cnt--; ++it)
			(*it) = val;
		return f;
	}

	template <typename T>
	template <class InputIt>
	typename TArray<T>::iterator TArray<T>::insert(TArray<T>::const_iterator it, InputIt first, InputIt last)
	{
		iterator f = &ptr[it - ptr];
		SizeType cnt = last - first;
		if (!cnt) return f;
		if (VectorSize + cnt > ReserveSize)
		{
			ReserveSize = (VectorSize + cnt) << 2;
			reallocate();
		}
		memmove(f + cnt, f, (VectorSize - (it - ptr)) * sizeof(T));
		for (iterator itr = f; first != last; ++it, ++first)
		{
			(*itr) = *first;
		}
		VectorSize += cnt;
		return f;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::insert(typename TArray<T>::const_iterator it, std::initializer_list<T> lst) {
		SizeType cnt = lst.size();
		iterator f = &ptr[it - ptr];
		if (!cnt) return f;
		if (VectorSize + cnt > ReserveSize)
		{
			ReserveSize = (VectorSize + cnt) << 2;
			reallocate();
		}
		memmove(f + cnt, f, (VectorSize - (it - ptr)) * sizeof(T));
		iterator iit = f;
		for (auto &item: lst)
		{
			(*iit) = item;
			++iit;
		}
		VectorSize += cnt;
		return f;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::erase(typename TArray<T>::const_iterator it)
	{
		iterator iit = &ptr[it - ptr];
		(*iit).~T();
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(T));
		--VectorSize;
		return iit;
	}

	template <typename T>
	typename TArray<T>::iterator TArray<T>::erase(typename TArray<T>::const_iterator first, typename TArray<T>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		for ( ; first != last; ++first)
			(*first).~T();
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(T));
		VectorSize -= last - first;
		return f;
	}

	template <typename T>
	void TArray<T>::swap(TArray<T> &rhs) noexcept
	{
		size_t tVectorSize = VectorSize,
			   tReserveSize = ReserveSize;
		T *tptr = ptr;

		VectorSize = rhs.VectorSize;
		ReserveSize = rhs.ReserveSize;
		ptr = rhs.ptr;

		rhs.VectorSize = tVectorSize;
		rhs.ReserveSize = tReserveSize;
		rhs.ptr = tptr;
	}

	template <typename T>
	void TArray<T>::clear() noexcept {
		SizeType i;
		for (i = 0; i < VectorSize; ++i)
			ptr[i].~T();
		VectorSize = 0;
	}

	
	template <typename T>
	bool TArray<T>::operator == (const TArray<T> &rhs) const {
		if (VectorSize != rhs.VectorSize) return false;
		SizeType i;
		for (i = 0; i < VectorSize; ++i)
			if (ptr[i] != rhs.ptr[i])
				return false;
		return true;
	}

	template <typename T>
	bool TArray<T>::operator != (const TArray<T> &rhs) const {
		if (VectorSize != rhs.VectorSize) return true;
		SizeType i;
		for (i = 0; i < VectorSize; ++i)
			if (ptr[i] != rhs.ptr[i])
				return true;
		return false;
	}

	template <typename T>
	bool TArray<T>::operator < (const TArray<T> &rhs) const {
		SizeType i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] < rhs.ptr[i];
		return VectorSize < rhs.VectorSize;
	}

	template <typename T>
	bool TArray<T>::operator <= (const TArray<T> &rhs) const {
		SizeType i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] < rhs.ptr[i];
		return VectorSize <= rhs.VectorSize;
	}

	template <typename T>
	bool TArray<T>::operator > (const TArray<T> &rhs) const {
		SizeType i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] > rhs.ptr[i];
		return VectorSize > rhs.VectorSize;
	}

	template <typename T>
	bool TArray<T>::operator >= (const TArray<T> &rhs) const {
		SizeType i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] > rhs.ptr[i];
		return VectorSize >= rhs.VectorSize;
	}



	template <>
	inline void TArray<bool>::resize(typename TArray<bool>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<signed char>::resize(typename TArray<signed char>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned char>::resize(typename TArray<unsigned char>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<char>::resize(typename TArray<char>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<short int>::resize(typename TArray<short int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned short int>::resize(typename TArray<unsigned short int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<int>::resize(typename TArray<int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned int>::resize(typename TArray<unsigned int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long int>::resize(typename TArray<long int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned long int>::resize(typename TArray<unsigned long int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long long int>::resize(typename TArray<long long int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned long long int>::resize(typename TArray<unsigned long long int>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<float>::resize(typename TArray<float>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<double>::resize(typename TArray<double>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long double>::resize(typename TArray<long double>::SizeType sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}


	template <>
	inline void TArray<bool>::resize(typename TArray<bool>::SizeType sz, const bool &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<signed char>::resize(typename TArray<signed char>::SizeType sz, const signed char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned char>::resize(typename TArray<unsigned char>::SizeType sz, const unsigned char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<char>::resize(typename TArray<char>::SizeType sz, const char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<short int>::resize(typename TArray<short int>::SizeType sz, const short int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned short int>::resize(typename TArray<unsigned short int>::SizeType sz, const unsigned short int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<int>::resize(typename TArray<int>::SizeType sz, const int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned int>::resize(typename TArray<unsigned int>::SizeType sz, const unsigned int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long int>::resize(typename TArray<long int>::SizeType sz, const long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned long int>::resize(typename TArray<unsigned long int>::SizeType sz, const unsigned long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long long int>::resize(typename TArray<long long int>::SizeType sz, const long long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<unsigned long long int>::resize(typename TArray<unsigned long long int>::SizeType sz, const unsigned long long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<float>::resize(typename TArray<float>::SizeType sz, const float &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<double>::resize(typename TArray<double>::SizeType sz, const double &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TArray<long double>::resize(typename TArray<long double>::SizeType sz, const long double &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			SizeType i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}


	template <>
	inline void TArray<bool>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<signed char>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<unsigned char>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<char>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<short int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<unsigned short int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<unsigned int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<long int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<unsigned long int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<long long int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<unsigned long long int>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<float>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<double>::PopBack() {
		--VectorSize;
	}

	template <>
	inline void TArray<long double>::PopBack() {
		--VectorSize;
	}


	template <>
	inline typename TArray<bool>::iterator TArray<bool>::erase(typename TArray<bool>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(bool));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<signed char>::iterator TArray<signed char>::erase(typename TArray<signed char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(signed char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<unsigned char>::iterator TArray<unsigned char>::erase(typename TArray<unsigned char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<char>::iterator TArray<char>::erase(typename TArray<char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<short int>::iterator TArray<short int>::erase(typename TArray<short int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(short int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<unsigned short int>::iterator TArray<unsigned short int>::erase(typename TArray<unsigned short int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned short int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<int>::iterator TArray<int>::erase(typename TArray<int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<unsigned int>::iterator TArray<unsigned int>::erase(typename TArray<unsigned int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<long int>::iterator TArray<long int>::erase(typename TArray<long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<unsigned long int>::iterator TArray<unsigned long int>::erase(typename TArray<unsigned long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<long long int>::iterator TArray<long long int>::erase(typename TArray<long long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<unsigned long long int>::iterator TArray<unsigned long long int>::erase(typename TArray<unsigned long long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned long long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<float>::iterator TArray<float>::erase(typename TArray<float>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(float));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<double>::iterator TArray<double>::erase(typename TArray<double>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(double));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TArray<long double>::iterator TArray<long double>::erase(typename TArray<long double>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long double));
		--VectorSize;
		return iit;
	}


	template <>
	inline typename TArray<bool>::iterator TArray<bool>::erase(typename TArray<bool>::const_iterator first, typename TArray<bool>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(bool));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<signed char>::iterator TArray<signed char>::erase(typename TArray<signed char>::const_iterator first, typename TArray<signed char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(signed char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<unsigned char>::iterator TArray<unsigned char>::erase(typename TArray<unsigned char>::const_iterator first, typename TArray<unsigned char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<char>::iterator TArray<char>::erase(typename TArray<char>::const_iterator first, typename TArray<char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<short int>::iterator TArray<short int>::erase(typename TArray<short int>::const_iterator first, typename TArray<short int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(short int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<unsigned short int>::iterator TArray<unsigned short int>::erase(typename TArray<unsigned short int>::const_iterator first, typename TArray<unsigned short int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned short int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<int>::iterator TArray<int>::erase(typename TArray<int>::const_iterator first, typename TArray<int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<unsigned int>::iterator TArray<unsigned int>::erase(typename TArray<unsigned int>::const_iterator first, typename TArray<unsigned int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<long long int>::iterator TArray<long long int>::erase(typename TArray<long long int>::const_iterator first, typename TArray<long long int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(long long int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<unsigned long long int>::iterator TArray<unsigned long long int>::erase(typename TArray<unsigned long long int>::const_iterator first, typename TArray<unsigned long long int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned long long int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<float>::iterator TArray<float>::erase(typename TArray<float>::const_iterator first, typename TArray<float>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(float));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<double>::iterator TArray<double>::erase(typename TArray<double>::const_iterator first, typename TArray<double>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(double));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TArray<long double>::iterator TArray<long double>::erase(typename TArray<long double>::const_iterator first, typename TArray<long double>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(long double));
		VectorSize -= last - first;
		return f;
	}


	template <>
	inline void TArray<bool>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<signed char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<unsigned char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<short int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<unsigned short int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<unsigned int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<unsigned long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<long long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<unsigned long long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <typename T>
	typename TArray<T>::SizeType TArray<T>::AddUnitialized(SizeType Count)
	{
		const SizeType OldNum = VectorSize;
		const SizeType NewNum = OldNum + Count;
		VectorSize = NewNum;
		ReserveSize = NewNum + 4;

		if (Count > OldNum)
		{
			resize(NewNum);
		}

		return VectorSize;
	}

	template <>
	inline void TArray<float>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<double>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TArray<long double>::clear() noexcept {
		VectorSize = 0;
	}
	
	template <typename T>
	void Print(const TArray<T> & v,std::string vec_name ) {
		 
		for(auto i = 0 ; i < v.size() ; i++) {
			 
			std::cout  << vec_name << "[" << i <<"] = " << v[i] << std::endl;
		}
		 
	}
	
	
	
}
#pragma warning(pop)