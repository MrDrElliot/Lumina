/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* List of builtins that Clang recognizes                                     *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/


#if defined(BUILTIN) && !defined(LIBBUILTIN)
#  define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, BUILTIN_LANG) BUILTIN(ID, TYPE, ATTRS)
#endif

#if defined(BUILTIN) && !defined(LANGBUILTIN)
#  define LANGBUILTIN(ID, TYPE, ATTRS, BUILTIN_LANG) BUILTIN(ID, TYPE, ATTRS)
#endif

// Some of our atomics builtins are handled by AtomicExpr rather than
// as normal builtin CallExprs. This macro is used for such builtins.
#ifndef ATOMIC_BUILTIN
#  define ATOMIC_BUILTIN(ID, TYPE, ATTRS) BUILTIN(ID, TYPE, ATTRS)
#endif

#if defined(BUILTIN) && !defined(TARGET_BUILTIN)
#  define TARGET_BUILTIN(ID, TYPE, ATTRS, FEATURE) BUILTIN(ID, TYPE, ATTRS)
#endif
TARGET_BUILTIN(__builtin_btf_type_id, "LUi.", "t", "")
TARGET_BUILTIN(__builtin_preserve_enum_value, "Li.", "t", "")
TARGET_BUILTIN(__builtin_preserve_field_info, "Ui.", "t", "")
TARGET_BUILTIN(__builtin_preserve_type_info, "LUi.", "t", "")

#undef ATOMIC_BUILTIN
#undef BUILTIN
#undef LIBBUILTIN
#undef LANGBUILTIN
#undef TARGET_BUILTIN
