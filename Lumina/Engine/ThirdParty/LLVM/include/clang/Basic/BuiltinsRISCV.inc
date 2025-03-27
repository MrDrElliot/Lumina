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
TARGET_BUILTIN(__builtin_riscv_aes32dsi, "UiUiUiIUi", "nc", "zknd,32bit")
TARGET_BUILTIN(__builtin_riscv_aes32dsmi, "UiUiUiIUi", "nc", "zknd,32bit")
TARGET_BUILTIN(__builtin_riscv_aes32esi, "UiUiUiIUi", "nc", "zkne,32bit")
TARGET_BUILTIN(__builtin_riscv_aes32esmi, "UiUiUiIUi", "nc", "zkne,32bit")
TARGET_BUILTIN(__builtin_riscv_aes64ds, "UWiUWiUWi", "nc", "zknd,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64dsm, "UWiUWiUWi", "nc", "zknd,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64es, "UWiUWiUWi", "nc", "zkne,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64esm, "UWiUWiUWi", "nc", "zkne,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64im, "UWiUWi", "nc", "zknd,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64ks1i, "UWiUWiIUi", "nc", "zknd|zkne,64bit")
TARGET_BUILTIN(__builtin_riscv_aes64ks2, "UWiUWiUWi", "nc", "zknd|zkne,64bit")
TARGET_BUILTIN(__builtin_riscv_brev8_32, "UiUi", "nc", "zbkb")
TARGET_BUILTIN(__builtin_riscv_brev8_64, "UWiUWi", "nc", "zbkb,64bit")
TARGET_BUILTIN(__builtin_riscv_clmul_32, "UiUiUi", "nc", "zbc|zbkc")
TARGET_BUILTIN(__builtin_riscv_clmul_64, "UWiUWiUWi", "nc", "zbc|zbkc,64bit")
TARGET_BUILTIN(__builtin_riscv_clmulh_32, "UiUiUi", "nc", "zbc|zbkc,32bit")
TARGET_BUILTIN(__builtin_riscv_clmulh_64, "UWiUWiUWi", "nc", "zbc|zbkc,64bit")
TARGET_BUILTIN(__builtin_riscv_clmulr_32, "UiUiUi", "nc", "zbc,32bit")
TARGET_BUILTIN(__builtin_riscv_clmulr_64, "UWiUWiUWi", "nc", "zbc,64bit")
TARGET_BUILTIN(__builtin_riscv_clz_32, "UiUi", "nc", "zbb|xtheadbb")
TARGET_BUILTIN(__builtin_riscv_clz_64, "UiUWi", "nc", "zbb|xtheadbb,64bit")
TARGET_BUILTIN(__builtin_riscv_ctz_32, "UiUi", "nc", "zbb")
TARGET_BUILTIN(__builtin_riscv_ctz_64, "UiUWi", "nc", "zbb,64bit")
TARGET_BUILTIN(__builtin_riscv_ntl_load, "v.", "t", "zihintntl")
TARGET_BUILTIN(__builtin_riscv_ntl_store, "v.", "t", "zihintntl")
TARGET_BUILTIN(__builtin_riscv_orc_b_32, "UiUi", "nc", "zbb")
TARGET_BUILTIN(__builtin_riscv_orc_b_64, "UWiUWi", "nc", "zbb,64bit")
TARGET_BUILTIN(__builtin_riscv_sha256sig0, "UiUi", "nc", "zknh")
TARGET_BUILTIN(__builtin_riscv_sha256sig1, "UiUi", "nc", "zknh")
TARGET_BUILTIN(__builtin_riscv_sha256sum0, "UiUi", "nc", "zknh")
TARGET_BUILTIN(__builtin_riscv_sha256sum1, "UiUi", "nc", "zknh")
TARGET_BUILTIN(__builtin_riscv_sha512sig0, "UWiUWi", "nc", "zknh,64bit")
TARGET_BUILTIN(__builtin_riscv_sha512sig0h, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sha512sig0l, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sha512sig1, "UWiUWi", "nc", "zknh,64bit")
TARGET_BUILTIN(__builtin_riscv_sha512sig1h, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sha512sig1l, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sha512sum0, "UWiUWi", "nc", "zknh,64bit")
TARGET_BUILTIN(__builtin_riscv_sha512sum0r, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sha512sum1, "UWiUWi", "nc", "zknh,64bit")
TARGET_BUILTIN(__builtin_riscv_sha512sum1r, "UiUiUi", "nc", "zknh,32bit")
TARGET_BUILTIN(__builtin_riscv_sm3p0, "UiUi", "nc", "zksh")
TARGET_BUILTIN(__builtin_riscv_sm3p1, "UiUi", "nc", "zksh")
TARGET_BUILTIN(__builtin_riscv_sm4ed, "UiUiUiIUi", "nc", "zksed")
TARGET_BUILTIN(__builtin_riscv_sm4ks, "UiUiUiIUi", "nc", "zksed")
TARGET_BUILTIN(__builtin_riscv_unzip_32, "UiUi", "nc", "zbkb,32bit")
TARGET_BUILTIN(__builtin_riscv_xperm4_32, "UiUiUi", "nc", "zbkx,32bit")
TARGET_BUILTIN(__builtin_riscv_xperm4_64, "UWiUWiUWi", "nc", "zbkx,64bit")
TARGET_BUILTIN(__builtin_riscv_xperm8_32, "UiUiUi", "nc", "zbkx,32bit")
TARGET_BUILTIN(__builtin_riscv_xperm8_64, "UWiUWiUWi", "nc", "zbkx,64bit")
TARGET_BUILTIN(__builtin_riscv_zip_32, "UiUi", "nc", "zbkb,32bit")

#undef ATOMIC_BUILTIN
#undef BUILTIN
#undef LIBBUILTIN
#undef LANGBUILTIN
#undef TARGET_BUILTIN
