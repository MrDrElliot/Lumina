#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/string.h>

#include "Memory/Memory.h"


namespace eastl
{
    allocator g_defaultAllocator;

    //-------------------------------------------------------------------------

    allocator* GetDefaultAllocator()
    {
        return &g_defaultAllocator;
    }

    allocator* SetDefaultAllocator(allocator* pAllocator)
    {
        return &g_defaultAllocator;
    }

    //-------------------------------------------------------------------------

    allocator::allocator(const char* EASTL_NAME(pName))
    {
        #if LE_DEBUG
        mpName = pName;
        #endif
    }

    allocator::allocator(const allocator& EASTL_NAME(alloc))
    {
        #if LE_DEBUG
        mpName = EASTL_ALLOCATOR_DEFAULT_NAME;
        #endif
    }

    allocator::allocator(const allocator&, const char* EASTL_NAME(pName))
    {
        #if LE_DEBUG
        mpName = pName;
        #endif
    }

    allocator& allocator::operator=(const allocator& EASTL_NAME(alloc))
    {
        return *this;
    }

    const char* allocator::get_name() const
    {
        return EASTL_ALLOCATOR_DEFAULT_NAME;
    }

    void allocator::set_name(const char* EASTL_NAME(pName))
    {
        // Implement set_name logic
    }

    void* allocator::allocate(size_t n, int flags)
    {
        return Lumina::Memory::Malloc(n, EASTL_ALLOCATOR_MIN_ALIGNMENT);
    }

    void* allocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
    {
        return Lumina::Memory::Malloc(n, alignment);
    }

    void allocator::deallocate(void* p, size_t)
    {
        Lumina::Memory::Free(p);
    }

    bool operator==(allocator const&, allocator const&) { return true; }
    bool operator!=(allocator const&, allocator const&) { return false; }
}

// Required for EASTL debug operator new[]
void* operator new[](size_t size, const char* pName, int flags, unsigned int debugFlags, const char* file, int line) 
{
    return Lumina::Memory::Malloc(size);
}

// Custom definition of aligned new[] operator for EASTL
void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    EASTL_ASSERT(alignment <= 8); // Ensure alignment is within supported limits
    return Lumina::Memory::Malloc(size, alignment); // Using system new for the aligned allocation
}

#if !EASTL_EASTDC_VSNPRINTF
// Custom Vsnprintf implementation for platforms that do not have EASTL's VSnprintf
int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
#ifdef _MSC_VER
    return _vsnprintf(pDestination, n, pFormat, arguments);
#else
    return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
    return _vsnwprintf((wchar_t*)pDestination, n, (wchar_t*)pFormat, arguments);
#else
    char* d = new char[n + 1];
    int r = vsnprintf(d, n, convertstring<char16_t, char>(pFormat).c_str(), arguments);
    memcpy(pDestination, convertstring<char, char16_t>(d).c_str(), (n + 1) * sizeof(char16_t));
    delete[] d;
    return r;
#endif
}
#endif // !EASTL_EASTDC_VSNPRINTF

