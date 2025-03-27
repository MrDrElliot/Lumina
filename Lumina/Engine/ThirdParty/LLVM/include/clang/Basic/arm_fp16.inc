#ifdef GET_NEON_BUILTINS
TARGET_BUILTIN(__builtin_neon_vabdh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vabsh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vaddh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcageh_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcagth_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcaleh_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcalth_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vceqh_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vceqzh_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcgeh_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcgezh_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcgth_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcgtzh_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcleh_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vclezh_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vclth_f16, "Ushh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcltzh_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_s16_f16, "sh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_s32_f16, "ih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_s64_f16, "Wih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_u16_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_u32_f16, "Uih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtah_u64_f16, "UWih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_s16, "hs", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_s32, "hi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_s64, "hWi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_u16, "hUs", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_u32, "hUi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_f16_u64, "hUWi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_s16, "hsIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_s32, "hiIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_s64, "hWiIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_u16, "hUsIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_u32, "hUiIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_f16_u64, "hUWiIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_s16_f16, "shIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_s32_f16, "ihIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_s64_f16, "WihIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_u16_f16, "UshIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_u32_f16, "UihIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_n_u64_f16, "UWihIi", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_s16_f16, "sh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_s32_f16, "ih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_s64_f16, "Wih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_u16_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_u32_f16, "Uih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvth_u64_f16, "UWih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_s16_f16, "sh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_s32_f16, "ih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_s64_f16, "Wih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_u16_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_u32_f16, "Uih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtmh_u64_f16, "UWih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_s16_f16, "sh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_s32_f16, "ih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_s64_f16, "Wih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_u16_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_u32_f16, "Uih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtnh_u64_f16, "UWih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_s16_f16, "sh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_s32_f16, "ih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_s64_f16, "Wih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_u16_f16, "Ush", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_u32_f16, "Uih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vcvtph_u64_f16, "UWih", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vdivh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vfmah_f16, "hhhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vfmsh_f16, "hhhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vmaxh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vmaxnmh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vminh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vminnmh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vmulh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vmulxh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vnegh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrecpeh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrecpsh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrecpxh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndah_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndih_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndmh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndnh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndph_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrndxh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrsqrteh_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vrsqrtsh_f16, "hhh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vsqrth_f16, "hh", "n", "fullfp16")
TARGET_BUILTIN(__builtin_neon_vsubh_f16, "hhh", "n", "fullfp16")
#endif

#ifdef GET_NEON_OVERLOAD_CHECK
#endif

#ifdef GET_NEON_STREAMING_COMPAT_FLAG
case NEON::BI__builtin_neon_vdivh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vmaxnmh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vminnmh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndah_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndih_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndmh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndnh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndph_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndxh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrndh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vsqrth_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vmulxh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vabdh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vabsh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vceqh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vceqzh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcgeh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcgezh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcgth_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcgtzh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcleh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vclezh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vclth_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcltzh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcageh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcagth_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcaleh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcalth_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtah_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtmh_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtnh_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvtph_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_s32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_s64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_s16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_u32_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_u64_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_u16_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrecpeh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrecpsh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrecpxh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrsqrteh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vrsqrtsh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_u16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_s16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_u32: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_s32: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u32: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s32: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_u64: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_f16_s64: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u64: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s64: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vaddh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vfmah_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vfmsh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vmaxh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vminh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vmulh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vnegh_f16: BuiltinType = ArmNonStreaming; break;
case NEON::BI__builtin_neon_vsubh_f16: BuiltinType = ArmNonStreaming; break;
#endif

#ifdef GET_NEON_IMMEDIATE_CHECK
case NEON::BI__builtin_neon_vcvth_n_s32_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_s64_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_s16_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_u32_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_u64_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_u16_f16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u32: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s32: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u64: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s64: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_u16: i = 1; l = 1; u = 15; break;
case NEON::BI__builtin_neon_vcvth_n_f16_s16: i = 1; l = 1; u = 15; break;
#endif

