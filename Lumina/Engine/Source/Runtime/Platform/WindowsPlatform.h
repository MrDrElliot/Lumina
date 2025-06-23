#pragma once

// Function type macros.
#define VARARGS     __cdecl											/* Functions with variable arguments */
#define STDCALL		__stdcall										/* Standard calling convention */

#define INLINE inline                                               /* Inline */
#define FORCEINLINE inline									        /* We no longer support __forceinline */
#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */


// DLL export and import definitions
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)


