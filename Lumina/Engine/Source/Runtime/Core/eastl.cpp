#include <EASTL/string.h>


// EASTL expects us to define these, see allocator.h line 194
void* operator new[](size_t size, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return ::new char[size];
}

void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    // this allocator doesn't support alignment
    EASTL_ASSERT(alignment <= 8);
    return ::new char[size];
}

// EASTL also wants us to define this (see string.h line 197)
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
    char* d = new char[n+1];
    int r = vsnprintf(d, n, convertstring<char16_t, char>(pFormat).c_str(), arguments);
    memcpy(pDestination, convertstring<char, char16_t>(d).c_str(), (n+1)*sizeof(char16_t));
    delete[] d;
    return r;
#endif
}