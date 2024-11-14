#pragma once

#include <iostream>
#include "Core/Assertions/Assert.h"

#define TFastVectorMaxSize UINT32_MAX
#pragma warning(push)
#pragma warning(disable :4244)
#pragma warning(disable: 4267)

namespace Lumina
{
	template <typename T>
	class TFastVector
	{
		public:
			// types:
			typedef T                                     value_type;
			typedef T &                                   reference;
			typedef const T &                             const_reference;
			typedef T *                                   pointer;
			typedef const T *                             const_pointer;
			typedef T *                                   iterator;
			typedef const T *                             const_iterator;
			typedef std::reverse_iterator<iterator>       reverse_iterator;
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
			typedef ptrdiff_t                             difference_type;
			typedef unsigned int                          size_type;

			// 23.3.11.2, construct/copy/destroy:
			TFastVector() noexcept;
			explicit TFastVector(size_type n);
			TFastVector(size_type n, const T &val);
			template <class InputIt> TFastVector(InputIt first, InputIt last); //v1(v2.begin(),v2.end())
			TFastVector(std::initializer_list<T>);
			TFastVector(const TFastVector<T> &);
			TFastVector(TFastVector<T> &&) noexcept;
			~TFastVector();
			TFastVector<T> & operator = (const TFastVector<T> &);
			TFastVector<T> & operator = (TFastVector<T> &&) noexcept;
			TFastVector<T> & operator = (std::initializer_list<T>);
			void assign(size_type, const T &value);
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
			size_type size() const noexcept;
			size_type max_size() const noexcept;
			size_type capacity() const noexcept;
			void resize(size_type);
			void resize(size_type, const T &);
			void reserve(size_type);
			void shrink_to_fit();

			// element access
			reference operator [](size_type);
			const_reference operator [](size_type) const;
			reference at(size_type);
			const_reference at(size_type) const;
			reference front();
			const_reference front() const;
			reference back();
			const_reference back() const;

			// 23.3.11.4, data access:
			T * data() noexcept;
			const T * data() const noexcept; 

			// 23.3.11.5, modifiers:
			template <class ... Args> void emplace_back(Args && ... args);
			void push_back(const T &);
			void push_back(T &&);
			void pop_back();
		
			template <class ... Args> iterator emplace(const_iterator, Args && ...); 
			iterator insert(const_iterator, const T &);
			iterator insert(const_iterator, T &&);
			iterator insert(const_iterator, size_type, const T&);
			template <class InputIt> iterator insert(const_iterator, InputIt, InputIt);
			iterator insert(const_iterator, std::initializer_list<T>);
			iterator erase(const_iterator);
			iterator erase(const_iterator, const_iterator);
			void swap(TFastVector<T> &) noexcept;
			void clear() noexcept;

			bool operator == (const TFastVector<T> &) const;
			bool operator != (const TFastVector<T> &) const;
			bool operator < (const TFastVector<T> &) const;
			bool operator <= (const TFastVector<T> &) const;
			bool operator > (const TFastVector<T> &) const;
			bool operator >= (const TFastVector<T> &) const;
			
			friend void Print(const TFastVector<T> &, std::string);
		private:
			size_type ReserveSize = 4;
			size_type VectorSize = 0;
			T *ptr;

			inline void reallocate();

	};


	
	template <typename T>
	TFastVector<T>::TFastVector() noexcept
	{
		ptr = new T[ReserveSize];
	}

	template <typename T>
	TFastVector<T>::TFastVector(TFastVector<T>::size_type n)
	{
		size_type i;
		ReserveSize = n << 2;
		ptr = new T[ReserveSize];
		for (i = 0; i < n; ++i)
		{
			ptr[i] = T();
		}
		VectorSize = n;
	}

	template <typename T>
	TFastVector<T>::TFastVector(typename TFastVector<T>::size_type n, const T &value)
	{
		size_type i;
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
	TFastVector<T>::TFastVector(InputIt first, InputIt last)
	{
		size_type i, count = last - first;
		ReserveSize = count << 2;
		VectorSize = count;
		ptr = new T[ReserveSize];
		for (i = 0; i < count; ++i, ++first)
		{
			ptr[i] = *first;
		}
	}

	template <typename T>
	TFastVector<T>::TFastVector(std::initializer_list<T> lst)
	{
		ReserveSize = lst.size() << 2;
		ptr = new T[ReserveSize];
		for (auto &item: lst)
		{
			ptr[VectorSize++] = item;
		}
	}

	template <typename T>
	TFastVector<T>::TFastVector(const TFastVector<T> &other)
	{
		size_type i;
		ReserveSize = other.ReserveSize;
		ptr = new T[ReserveSize];
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = other.ptr[i];
		}
		VectorSize = other.VectorSize;
	}

	template <typename T>
	TFastVector<T>::TFastVector(TFastVector<T> &&other) noexcept
	{
		size_type i;
		ReserveSize = other.ReserveSize;
		ptr = new T[ReserveSize];
		for (i = 0; i < other.VectorSize; ++i)
		{
			ptr[i] = std::move(other.ptr[i]);
		}
		VectorSize = other.VectorSize;
	}

	template <typename T>
	TFastVector<T>::~TFastVector()
	{
		delete [] ptr;
	}

	template <typename T>
	TFastVector<T> & TFastVector<T>::operator = (const TFastVector<T> &other)
	{
		if(this == &other)
		{
			return *this;
		}
		size_type i;
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
	TFastVector<T>& TFastVector<T>::operator = (TFastVector<T> &&other) noexcept
	{
		size_type i;
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
	TFastVector<T> & TFastVector<T>::operator = (std::initializer_list<T> lst) {
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
	void TFastVector<T>::assign(typename TFastVector<T>::size_type count, const T &value) {
		size_type i;
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
	void TFastVector<T>::assign(InputIt first, InputIt last) {
		size_type i, count = last - first;
		if (count > ReserveSize) {
			ReserveSize = count << 2;
			reallocate();
		}
		for (i = 0; i < count; ++i, ++first)
			ptr[i] = *first;
		VectorSize = count;
	}

	template <typename T>
	void TFastVector<T>::assign(std::initializer_list<T> lst) {
		size_type i, count = lst.size();
		if (count > ReserveSize) {
			ReserveSize = count << 2;
			reallocate();
		}
		i = 0;
		for (auto &item: lst)
			ptr[i++] = item;
	}


	template <typename T>
	typename TFastVector<T>::iterator TFastVector<T>::begin() noexcept {
		return ptr;
	}

	template <typename T>
	typename TFastVector<T>::const_iterator TFastVector<T>::begin() const noexcept {
		return ptr;
	}

	template <typename T>
	typename TFastVector<T>::iterator TFastVector<T>::end() noexcept {
		return ptr + VectorSize;
	}

	template <typename T>
	typename TFastVector<T>::const_iterator TFastVector<T>::end() const noexcept {
		return ptr + VectorSize;
	}

	template <typename T>
	typename TFastVector<T>::reverse_iterator TFastVector<T>::rbegin() noexcept {
		return reverse_iterator(ptr + VectorSize);
	}

	template <typename T>
	typename TFastVector<T>::const_reverse_iterator TFastVector<T>::crbegin() const noexcept {
		return reverse_iterator(ptr + VectorSize);
	}

	template <typename T>
	typename TFastVector<T>::reverse_iterator TFastVector<T>::rend() noexcept {
		return reverse_iterator(ptr);
	}
	
	template <typename T>
	typename TFastVector<T>::const_reverse_iterator TFastVector<T>::crend() const noexcept {
		return reverse_iterator(ptr);
	}

	template <typename T>
	void TFastVector<T>::reallocate()
	{
		AssertMsg(VectorSize <= ReserveSize, "VectorSize must not exceed ReserveSize.");

		// Check for potential overflow
		//AssertMsg(VectorSize <= (std::numeric_limits<size_t>::max() / sizeof(T)), "VectorSize * sizeof(T) would overflow.");

		T* tptr = new T[ReserveSize];

		if constexpr (std::is_trivially_copyable<T>::value)
		{
			memcpy(tptr, ptr, VectorSize * sizeof(T));
		}
		else
		{
			for (size_type i = 0; i < VectorSize; ++i)
			{
				new (&tptr[i]) T(std::move(ptr[i]));
				ptr[i].~T();
			}
		}

		delete[] ptr;
		ptr = tptr;
	}


	template <typename T>
	bool TFastVector<T>::empty() const noexcept {
		return VectorSize == 0;
	}
	
	template <typename T>
	typename TFastVector<T>::size_type TFastVector<T>::size() const noexcept
	{
		return VectorSize;
	}

	template <typename T>
	typename TFastVector<T>::size_type TFastVector<T>::max_size() const noexcept
	{
		return TFastVectorMaxSize;
	}

	template <typename T>
	typename TFastVector<T>::size_type TFastVector<T>::capacity() const noexcept
	{
		return ReserveSize;
	}
	
	template <typename T>
	void TFastVector<T>::resize(typename TFastVector<T>::size_type sz) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
		} else {
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i].~T();
		}
		VectorSize = sz;
	}

	template <typename T>
	void TFastVector<T>::resize(typename TFastVector<T>::size_type sz, const T &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		} else {
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i].~T();
		}
		VectorSize = sz;
	}

	template <typename T>
	void TFastVector<T>::reserve(TFastVector<T>::size_type _sz)
	{
		if (_sz > ReserveSize)
		{
			ReserveSize = _sz;
			reallocate();
		}
	}

	template <typename T>
	void TFastVector<T>::shrink_to_fit()
	{
		ReserveSize = VectorSize;
		reallocate();
	}

	
	template <typename T>
	typename TFastVector<T>::reference TFastVector<T>::operator [](TFastVector<T>::size_type idx) {
		return ptr[idx];
	}

	template <typename T>
	typename TFastVector<T>::const_reference TFastVector<T>::operator [](TFastVector<T>::size_type idx) const {
		return ptr[idx];
	}

	template <typename T>
	typename TFastVector<T>::reference TFastVector<T>::at(size_type pos) {
		if (pos < VectorSize)
			return ptr[pos];
		else
			throw std::out_of_range("accessed position is out of range");
	}

	template <typename T>
	typename TFastVector<T>::const_reference TFastVector<T>::at(size_type pos) const {
		if (pos < VectorSize)
			return ptr[pos];
		else
			throw std::out_of_range("accessed position is out of range");
	}

	template <typename T>
	typename TFastVector<T>::reference TFastVector<T>::front() {
		return ptr[0];
	}

	template <typename T>
	typename TFastVector<T>::const_reference TFastVector<T>::front() const {
		return ptr[0];
	}

	template <typename T>
	typename TFastVector<T>::reference TFastVector<T>::back() {
		return ptr[VectorSize - 1];
	}

	template <typename T>
	typename TFastVector<T>::const_reference TFastVector<T>::back() const
	{
		return ptr[VectorSize - 1];
	}

	
	template <typename T>
	T* TFastVector<T>::data() noexcept
	{
		return ptr;
	}

	template <typename T>
	const T* TFastVector<T>::data() const noexcept
	{
		return ptr;
	}


	template <typename T>
	template <class ... Args>
	void TFastVector<T>::emplace_back(Args && ... args)
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
	void TFastVector<T>::push_back(const T &val) {
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		ptr[VectorSize] = val;
		++VectorSize;
	}

	template <typename T>
	void TFastVector<T>::push_back(T &&val) {
		if (VectorSize == ReserveSize) {
			ReserveSize <<= 2;
			reallocate();
		}
		ptr[VectorSize] = std::move(val);
		++VectorSize;
	}

	template <typename T>
	void TFastVector<T>::pop_back() {
		--VectorSize;
		ptr[VectorSize].~T();
	}


	template <typename T>
	template <class ... Args>
	typename TFastVector<T>::iterator TFastVector<T>::emplace(typename TFastVector<T>::const_iterator it, Args && ... args) {
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
	typename TFastVector<T>::iterator TFastVector<T>::insert(typename TFastVector<T>::const_iterator it, const T &val) {
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
	typename TFastVector<T>::iterator TFastVector<T>::insert(typename TFastVector<T>::const_iterator it, T &&val)
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
	typename TFastVector<T>::iterator TFastVector<T>::insert(TFastVector<T>::const_iterator it, TFastVector<T>::size_type cnt, const T &val) {
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
	typename TFastVector<T>::iterator TFastVector<T>::insert(TFastVector<T>::const_iterator it, InputIt first, InputIt last)
	{
		iterator f = &ptr[it - ptr];
		size_type cnt = last - first;
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
	typename TFastVector<T>::iterator TFastVector<T>::insert(typename TFastVector<T>::const_iterator it, std::initializer_list<T> lst) {
		size_type cnt = lst.size();
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
	typename TFastVector<T>::iterator TFastVector<T>::erase(typename TFastVector<T>::const_iterator it)
	{
		iterator iit = &ptr[it - ptr];
		(*iit).~T();
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(T));
		--VectorSize;
		return iit;
	}

	template <typename T>
	typename TFastVector<T>::iterator TFastVector<T>::erase(typename TFastVector<T>::const_iterator first, typename TFastVector<T>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		for ( ; first != last; ++first)
			(*first).~T();
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(T));
		VectorSize -= last - first;
		return f;
	}

	template <typename T>
	void TFastVector<T>::swap(TFastVector<T> &rhs) noexcept
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
	void TFastVector<T>::clear() noexcept {
		size_type i;
		for (i = 0; i < VectorSize; ++i)
			ptr[i].~T();
		VectorSize = 0;
	}

	
	template <typename T>
	bool TFastVector<T>::operator == (const TFastVector<T> &rhs) const {
		if (VectorSize != rhs.VectorSize) return false;
		size_type i;
		for (i = 0; i < VectorSize; ++i)
			if (ptr[i] != rhs.ptr[i])
				return false;
		return true;
	}

	template <typename T>
	bool TFastVector<T>::operator != (const TFastVector<T> &rhs) const {
		if (VectorSize != rhs.VectorSize) return true;
		size_type i;
		for (i = 0; i < VectorSize; ++i)
			if (ptr[i] != rhs.ptr[i])
				return true;
		return false;
	}

	template <typename T>
	bool TFastVector<T>::operator < (const TFastVector<T> &rhs) const {
		size_type i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] < rhs.ptr[i];
		return VectorSize < rhs.VectorSize;
	}

	template <typename T>
	bool TFastVector<T>::operator <= (const TFastVector<T> &rhs) const {
		size_type i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] < rhs.ptr[i];
		return VectorSize <= rhs.VectorSize;
	}

	template <typename T>
	bool TFastVector<T>::operator > (const TFastVector<T> &rhs) const {
		size_type i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] > rhs.ptr[i];
		return VectorSize > rhs.VectorSize;
	}

	template <typename T>
	bool TFastVector<T>::operator >= (const TFastVector<T> &rhs) const {
		size_type i, j, ub = VectorSize < rhs.VectorSize ? VectorSize : rhs.VectorSize;
		for (i = 0; i < ub; ++i)
			if (ptr[i] != rhs.ptr[i])
				return ptr[i] > rhs.ptr[i];
		return VectorSize >= rhs.VectorSize;
	}



	template <>
	inline void TFastVector<bool>::resize(typename TFastVector<bool>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<signed char>::resize(typename TFastVector<signed char>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned char>::resize(typename TFastVector<unsigned char>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<char>::resize(typename TFastVector<char>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<short int>::resize(typename TFastVector<short int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned short int>::resize(typename TFastVector<unsigned short int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<int>::resize(typename TFastVector<int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned int>::resize(typename TFastVector<unsigned int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long int>::resize(typename TFastVector<long int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned long int>::resize(typename TFastVector<unsigned long int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long long int>::resize(typename TFastVector<long long int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned long long int>::resize(typename TFastVector<unsigned long long int>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<float>::resize(typename TFastVector<float>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<double>::resize(typename TFastVector<double>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long double>::resize(typename TFastVector<long double>::size_type sz) {
		if (sz > ReserveSize) {
			ReserveSize = sz;
			reallocate();
		}
		VectorSize = sz;
	}


	template <>
	inline void TFastVector<bool>::resize(typename TFastVector<bool>::size_type sz, const bool &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<signed char>::resize(typename TFastVector<signed char>::size_type sz, const signed char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned char>::resize(typename TFastVector<unsigned char>::size_type sz, const unsigned char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<char>::resize(typename TFastVector<char>::size_type sz, const char &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<short int>::resize(typename TFastVector<short int>::size_type sz, const short int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned short int>::resize(typename TFastVector<unsigned short int>::size_type sz, const unsigned short int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<int>::resize(typename TFastVector<int>::size_type sz, const int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned int>::resize(typename TFastVector<unsigned int>::size_type sz, const unsigned int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long int>::resize(typename TFastVector<long int>::size_type sz, const long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned long int>::resize(typename TFastVector<unsigned long int>::size_type sz, const unsigned long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long long int>::resize(typename TFastVector<long long int>::size_type sz, const long long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<unsigned long long int>::resize(typename TFastVector<unsigned long long int>::size_type sz, const unsigned long long int &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<float>::resize(typename TFastVector<float>::size_type sz, const float &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<double>::resize(typename TFastVector<double>::size_type sz, const double &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}

	template <>
	inline void TFastVector<long double>::resize(typename TFastVector<long double>::size_type sz, const long double &c) {
		if (sz > VectorSize) {
			if (sz > ReserveSize) {
				ReserveSize = sz;
				reallocate();
			}
			size_type i;
			for (i = VectorSize; i < sz; ++i)
				ptr[i] = c;
		}
		VectorSize = sz;
	}


	template <>
	inline void TFastVector<bool>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<signed char>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<unsigned char>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<char>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<short int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<unsigned short int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<unsigned int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<long int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<unsigned long int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<long long int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<unsigned long long int>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<float>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<double>::pop_back() {
		--VectorSize;
	}

	template <>
	inline void TFastVector<long double>::pop_back() {
		--VectorSize;
	}


	template <>
	inline typename TFastVector<bool>::iterator TFastVector<bool>::erase(typename TFastVector<bool>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(bool));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<signed char>::iterator TFastVector<signed char>::erase(typename TFastVector<signed char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(signed char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<unsigned char>::iterator TFastVector<unsigned char>::erase(typename TFastVector<unsigned char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<char>::iterator TFastVector<char>::erase(typename TFastVector<char>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(char));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<short int>::iterator TFastVector<short int>::erase(typename TFastVector<short int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(short int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<unsigned short int>::iterator TFastVector<unsigned short int>::erase(typename TFastVector<unsigned short int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned short int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<int>::iterator TFastVector<int>::erase(typename TFastVector<int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<unsigned int>::iterator TFastVector<unsigned int>::erase(typename TFastVector<unsigned int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<long int>::iterator TFastVector<long int>::erase(typename TFastVector<long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<unsigned long int>::iterator TFastVector<unsigned long int>::erase(typename TFastVector<unsigned long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<long long int>::iterator TFastVector<long long int>::erase(typename TFastVector<long long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<unsigned long long int>::iterator TFastVector<unsigned long long int>::erase(typename TFastVector<unsigned long long int>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(unsigned long long int));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<float>::iterator TFastVector<float>::erase(typename TFastVector<float>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(float));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<double>::iterator TFastVector<double>::erase(typename TFastVector<double>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(double));
		--VectorSize;
		return iit;
	}

	template <>
	inline typename TFastVector<long double>::iterator TFastVector<long double>::erase(typename TFastVector<long double>::const_iterator it) {
		iterator iit = &ptr[it - ptr];
		memmove(iit, iit + 1, (VectorSize - (it - ptr) - 1) * sizeof(long double));
		--VectorSize;
		return iit;
	}


	template <>
	inline typename TFastVector<bool>::iterator TFastVector<bool>::erase(typename TFastVector<bool>::const_iterator first, typename TFastVector<bool>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(bool));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<signed char>::iterator TFastVector<signed char>::erase(typename TFastVector<signed char>::const_iterator first, typename TFastVector<signed char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(signed char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<unsigned char>::iterator TFastVector<unsigned char>::erase(typename TFastVector<unsigned char>::const_iterator first, typename TFastVector<unsigned char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<char>::iterator TFastVector<char>::erase(typename TFastVector<char>::const_iterator first, typename TFastVector<char>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(char));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<short int>::iterator TFastVector<short int>::erase(typename TFastVector<short int>::const_iterator first, typename TFastVector<short int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(short int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<unsigned short int>::iterator TFastVector<unsigned short int>::erase(typename TFastVector<unsigned short int>::const_iterator first, typename TFastVector<unsigned short int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned short int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<int>::iterator TFastVector<int>::erase(typename TFastVector<int>::const_iterator first, typename TFastVector<int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<unsigned int>::iterator TFastVector<unsigned int>::erase(typename TFastVector<unsigned int>::const_iterator first, typename TFastVector<unsigned int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<long long int>::iterator TFastVector<long long int>::erase(typename TFastVector<long long int>::const_iterator first, typename TFastVector<long long int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(long long int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<unsigned long long int>::iterator TFastVector<unsigned long long int>::erase(typename TFastVector<unsigned long long int>::const_iterator first, typename TFastVector<unsigned long long int>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(unsigned long long int));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<float>::iterator TFastVector<float>::erase(typename TFastVector<float>::const_iterator first, typename TFastVector<float>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(float));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<double>::iterator TFastVector<double>::erase(typename TFastVector<double>::const_iterator first, typename TFastVector<double>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(double));
		VectorSize -= last - first;
		return f;
	}

	template <>
	inline typename TFastVector<long double>::iterator TFastVector<long double>::erase(typename TFastVector<long double>::const_iterator first, typename TFastVector<long double>::const_iterator last) {
		iterator f = &ptr[first - ptr];
		if (first == last) return f;
		memmove(f, last, (VectorSize - (last - ptr)) * sizeof(long double));
		VectorSize -= last - first;
		return f;
	}


	template <>
	inline void TFastVector<bool>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<signed char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<unsigned char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<char>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<short int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<unsigned short int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<unsigned int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<unsigned long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<long long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<unsigned long long int>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<float>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<double>::clear() noexcept {
		VectorSize = 0;
	}

	template <>
	inline void TFastVector<long double>::clear() noexcept {
		VectorSize = 0;
	}
	
	template <typename T>
	void Print(const TFastVector<T> & v,std::string vec_name ) {
		 
		for(auto i = 0 ; i < v.size() ; i++) {
			 
			std::cout  << vec_name << "[" << i <<"] = " << v[i] << std::endl;
		}
		 
	}
	
	

}
#pragma warning(pop)