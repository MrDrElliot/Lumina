#ifdef GET_SUPPORTED_EXTENSIONS
#undef GET_SUPPORTED_EXTENSIONS

static const RISCVSupportedExtension SupportedExtensions[] = {
    {"a", {2, 1}},
    {"c", {2, 0}},
    {"d", {2, 2}},
    {"e", {2, 0}},
    {"f", {2, 2}},
    {"h", {1, 0}},
    {"i", {2, 1}},
    {"m", {2, 0}},
    {"shcounterenw", {1, 0}},
    {"shgatpa", {1, 0}},
    {"shtvala", {1, 0}},
    {"shvsatpa", {1, 0}},
    {"shvstvala", {1, 0}},
    {"shvstvecd", {1, 0}},
    {"smaia", {1, 0}},
    {"smepmp", {1, 0}},
    {"smstateen", {1, 0}},
    {"ssaia", {1, 0}},
    {"ssccptr", {1, 0}},
    {"sscofpmf", {1, 0}},
    {"sscounterenw", {1, 0}},
    {"ssstateen", {1, 0}},
    {"ssstrict", {1, 0}},
    {"sstc", {1, 0}},
    {"sstvala", {1, 0}},
    {"sstvecd", {1, 0}},
    {"ssu64xl", {1, 0}},
    {"svade", {1, 0}},
    {"svadu", {1, 0}},
    {"svbare", {1, 0}},
    {"svinval", {1, 0}},
    {"svnapot", {1, 0}},
    {"svpbmt", {1, 0}},
    {"v", {1, 0}},
    {"xcvalu", {1, 0}},
    {"xcvbi", {1, 0}},
    {"xcvbitmanip", {1, 0}},
    {"xcvelw", {1, 0}},
    {"xcvmac", {1, 0}},
    {"xcvmem", {1, 0}},
    {"xcvsimd", {1, 0}},
    {"xsfcease", {1, 0}},
    {"xsfvcp", {1, 0}},
    {"xsfvfnrclipxfqf", {1, 0}},
    {"xsfvfwmaccqqq", {1, 0}},
    {"xsfvqmaccdod", {1, 0}},
    {"xsfvqmaccqoq", {1, 0}},
    {"xsifivecdiscarddlone", {1, 0}},
    {"xsifivecflushdlone", {1, 0}},
    {"xtheadba", {1, 0}},
    {"xtheadbb", {1, 0}},
    {"xtheadbs", {1, 0}},
    {"xtheadcmo", {1, 0}},
    {"xtheadcondmov", {1, 0}},
    {"xtheadfmemidx", {1, 0}},
    {"xtheadmac", {1, 0}},
    {"xtheadmemidx", {1, 0}},
    {"xtheadmempair", {1, 0}},
    {"xtheadsync", {1, 0}},
    {"xtheadvdot", {1, 0}},
    {"xventanacondops", {1, 0}},
    {"za128rs", {1, 0}},
    {"za64rs", {1, 0}},
    {"zaamo", {1, 0}},
    {"zacas", {1, 0}},
    {"zalrsc", {1, 0}},
    {"zama16b", {1, 0}},
    {"zawrs", {1, 0}},
    {"zba", {1, 0}},
    {"zbb", {1, 0}},
    {"zbc", {1, 0}},
    {"zbkb", {1, 0}},
    {"zbkc", {1, 0}},
    {"zbkx", {1, 0}},
    {"zbs", {1, 0}},
    {"zca", {1, 0}},
    {"zcb", {1, 0}},
    {"zcd", {1, 0}},
    {"zce", {1, 0}},
    {"zcf", {1, 0}},
    {"zcmop", {1, 0}},
    {"zcmp", {1, 0}},
    {"zcmt", {1, 0}},
    {"zdinx", {1, 0}},
    {"zfa", {1, 0}},
    {"zfh", {1, 0}},
    {"zfhmin", {1, 0}},
    {"zfinx", {1, 0}},
    {"zhinx", {1, 0}},
    {"zhinxmin", {1, 0}},
    {"zic64b", {1, 0}},
    {"zicbom", {1, 0}},
    {"zicbop", {1, 0}},
    {"zicboz", {1, 0}},
    {"ziccamoa", {1, 0}},
    {"ziccif", {1, 0}},
    {"zicclsm", {1, 0}},
    {"ziccrse", {1, 0}},
    {"zicntr", {2, 0}},
    {"zicond", {1, 0}},
    {"zicsr", {2, 0}},
    {"zifencei", {2, 0}},
    {"zihintntl", {1, 0}},
    {"zihintpause", {2, 0}},
    {"zihpm", {2, 0}},
    {"zimop", {1, 0}},
    {"zk", {1, 0}},
    {"zkn", {1, 0}},
    {"zknd", {1, 0}},
    {"zkne", {1, 0}},
    {"zknh", {1, 0}},
    {"zkr", {1, 0}},
    {"zks", {1, 0}},
    {"zksed", {1, 0}},
    {"zksh", {1, 0}},
    {"zkt", {1, 0}},
    {"zmmul", {1, 0}},
    {"zvbb", {1, 0}},
    {"zvbc", {1, 0}},
    {"zve32f", {1, 0}},
    {"zve32x", {1, 0}},
    {"zve64d", {1, 0}},
    {"zve64f", {1, 0}},
    {"zve64x", {1, 0}},
    {"zvfh", {1, 0}},
    {"zvfhmin", {1, 0}},
    {"zvkb", {1, 0}},
    {"zvkg", {1, 0}},
    {"zvkn", {1, 0}},
    {"zvknc", {1, 0}},
    {"zvkned", {1, 0}},
    {"zvkng", {1, 0}},
    {"zvknha", {1, 0}},
    {"zvknhb", {1, 0}},
    {"zvks", {1, 0}},
    {"zvksc", {1, 0}},
    {"zvksed", {1, 0}},
    {"zvksg", {1, 0}},
    {"zvksh", {1, 0}},
    {"zvkt", {1, 0}},
    {"zvl1024b", {1, 0}},
    {"zvl128b", {1, 0}},
    {"zvl16384b", {1, 0}},
    {"zvl2048b", {1, 0}},
    {"zvl256b", {1, 0}},
    {"zvl32768b", {1, 0}},
    {"zvl32b", {1, 0}},
    {"zvl4096b", {1, 0}},
    {"zvl512b", {1, 0}},
    {"zvl64b", {1, 0}},
    {"zvl65536b", {1, 0}},
    {"zvl8192b", {1, 0}},
};

static const RISCVSupportedExtension SupportedExperimentalExtensions[] = {
    {"smmpm", {0, 8}},
    {"smnpm", {0, 8}},
    {"ssnpm", {0, 8}},
    {"sspm", {0, 8}},
    {"ssqosid", {1, 0}},
    {"supm", {0, 8}},
    {"zabha", {1, 0}},
    {"zalasr", {0, 1}},
    {"zfbfmin", {1, 0}},
    {"zicfilp", {0, 4}},
    {"zicfiss", {0, 4}},
    {"ztso", {0, 1}},
    {"zvfbfmin", {1, 0}},
    {"zvfbfwma", {1, 0}},
};

#endif // GET_SUPPORTED_EXTENSIONS

#ifdef GET_IMPLIED_EXTENSIONS
#undef GET_IMPLIED_EXTENSIONS


static constexpr ImpliedExtsEntry ImpliedExts[] = {
    { {"d"}, "f"},
    { {"f"}, "zicsr"},
    { {"v"}, "zvl128b"},
    { {"v"}, "zve64d"},
    { {"xsfvcp"}, "zve32x"},
    { {"xsfvfnrclipxfqf"}, "zve32f"},
    { {"xsfvfwmaccqqq"}, "zvfbfmin"},
    { {"xsfvqmaccdod"}, "zve32x"},
    { {"xsfvqmaccqoq"}, "zve32x"},
    { {"xtheadvdot"}, "v"},
    { {"zcb"}, "zca"},
    { {"zcd"}, "d"},
    { {"zcd"}, "zca"},
    { {"zce"}, "zcb"},
    { {"zce"}, "zcmp"},
    { {"zce"}, "zcmt"},
    { {"zcf"}, "f"},
    { {"zcf"}, "zca"},
    { {"zcmop"}, "zca"},
    { {"zcmp"}, "zca"},
    { {"zcmt"}, "zca"},
    { {"zcmt"}, "zicsr"},
    { {"zdinx"}, "zfinx"},
    { {"zfa"}, "f"},
    { {"zfbfmin"}, "f"},
    { {"zfh"}, "zfhmin"},
    { {"zfhmin"}, "f"},
    { {"zfinx"}, "zicsr"},
    { {"zhinx"}, "zhinxmin"},
    { {"zhinxmin"}, "zfinx"},
    { {"zicfilp"}, "zicsr"},
    { {"zicfiss"}, "zicsr"},
    { {"zicfiss"}, "zimop"},
    { {"zicntr"}, "zicsr"},
    { {"zihpm"}, "zicsr"},
    { {"zk"}, "zkn"},
    { {"zk"}, "zkr"},
    { {"zk"}, "zkt"},
    { {"zkn"}, "zbkb"},
    { {"zkn"}, "zbkc"},
    { {"zkn"}, "zbkx"},
    { {"zkn"}, "zkne"},
    { {"zkn"}, "zknd"},
    { {"zkn"}, "zknh"},
    { {"zks"}, "zbkb"},
    { {"zks"}, "zbkc"},
    { {"zks"}, "zbkx"},
    { {"zks"}, "zksed"},
    { {"zks"}, "zksh"},
    { {"zvbb"}, "zvkb"},
    { {"zve32f"}, "zve32x"},
    { {"zve32f"}, "f"},
    { {"zve32x"}, "zicsr"},
    { {"zve32x"}, "zvl32b"},
    { {"zve64d"}, "zve64f"},
    { {"zve64d"}, "d"},
    { {"zve64f"}, "zve32f"},
    { {"zve64f"}, "zve64x"},
    { {"zve64x"}, "zve32x"},
    { {"zve64x"}, "zvl64b"},
    { {"zvfbfmin"}, "zve32f"},
    { {"zvfbfwma"}, "zvfbfmin"},
    { {"zvfbfwma"}, "zfbfmin"},
    { {"zvfh"}, "zvfhmin"},
    { {"zvfh"}, "zfhmin"},
    { {"zvfhmin"}, "zve32f"},
    { {"zvkn"}, "zvkned"},
    { {"zvkn"}, "zvknhb"},
    { {"zvkn"}, "zvkb"},
    { {"zvkn"}, "zvkt"},
    { {"zvknc"}, "zvkn"},
    { {"zvknc"}, "zvbc"},
    { {"zvkng"}, "zvkn"},
    { {"zvkng"}, "zvkg"},
    { {"zvknhb"}, "zve64x"},
    { {"zvks"}, "zvksed"},
    { {"zvks"}, "zvksh"},
    { {"zvks"}, "zvkb"},
    { {"zvks"}, "zvkt"},
    { {"zvksc"}, "zvks"},
    { {"zvksc"}, "zvbc"},
    { {"zvksg"}, "zvks"},
    { {"zvksg"}, "zvkg"},
    { {"zvl1024b"}, "zvl512b"},
    { {"zvl128b"}, "zvl64b"},
    { {"zvl16384b"}, "zvl8192b"},
    { {"zvl2048b"}, "zvl1024b"},
    { {"zvl256b"}, "zvl128b"},
    { {"zvl32768b"}, "zvl16384b"},
    { {"zvl4096b"}, "zvl2048b"},
    { {"zvl512b"}, "zvl256b"},
    { {"zvl64b"}, "zvl32b"},
    { {"zvl65536b"}, "zvl32768b"},
    { {"zvl8192b"}, "zvl4096b"},
};

#endif // GET_IMPLIED_EXTENSIONS

#ifdef GET_SUPPORTED_PROFILES
#undef GET_SUPPORTED_PROFILES

static constexpr RISCVProfile SupportedProfiles[] = {
    {"rva20s64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zifencei2p0_za128rs1p0_ssccptr1p0_sstvala1p0_sstvecd1p0_svade1p0_svbare1p0"},
    {"rva20u64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_za128rs1p0"},
    {"rva22s64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zifencei2p0_zihintpause2p0_zihpm2p0_za64rs1p0_zfhmin1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0_ssccptr1p0_sscounterenw1p0_sstvala1p0_sstvecd1p0_svade1p0_svbare1p0_svinval1p0_svpbmt1p0"},
    {"rva22u64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zihintpause2p0_zihpm2p0_za64rs1p0_zfhmin1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0"},
    {"rvi20u32","rv32i2p1"},
    {"rvi20u64","rv64i2p1"},
};

static constexpr RISCVProfile SupportedExperimentalProfiles[] = {
    {"rva23s64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_h1p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zicond1p0_zifencei2p0_zihintntl1p0_zihintpause2p0_zihpm2p0_zimop1p0_za64rs1p0_zawrs1p0_zfa1p0_zfhmin1p0_zcb1p0_zcmop1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0_zvbb1p0_zvfhmin1p0_zvkt1p0_shcounterenw1p0_shgatpa1p0_shtvala1p0_shvsatpa1p0_shvstvala1p0_shvstvecd1p0_ssccptr1p0_sscofpmf1p0_sscounterenw1p0_ssnpm0p8_ssstateen1p0_sstc1p0_sstvala1p0_sstvecd1p0_ssu64xl1p0_svade1p0_svbare1p0_svinval1p0_svnapot1p0_svpbmt1p0"},
    {"rva23u64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zicond1p0_zihintntl1p0_zihintpause2p0_zihpm2p0_zimop1p0_za64rs1p0_zawrs1p0_zfa1p0_zfhmin1p0_zcb1p0_zcmop1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0_zvbb1p0_zvfhmin1p0_zvkt1p0"},
    {"rvb23s64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zicond1p0_zifencei2p0_zihintntl1p0_zihintpause2p0_zihpm2p0_zimop1p0_za64rs1p0_zawrs1p0_zfa1p0_zcb1p0_zcmop1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0_ssccptr1p0_sscofpmf1p0_sscounterenw1p0_sstc1p0_sstvala1p0_sstvecd1p0_ssu64xl1p0_svade1p0_svbare1p0_svinval1p0_svnapot1p0_svpbmt1p0"},
    {"rvb23u64","rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zicntr2p0_zicond1p0_zihintntl1p0_zihintpause2p0_zihpm2p0_zimop1p0_za64rs1p0_zawrs1p0_zfa1p0_zcb1p0_zcmop1p0_zba1p0_zbb1p0_zbs1p0_zkt1p0"},
    {"rvm23u32","rv32i2p1_m2p0_zicbop1p0_zicond1p0_zihintntl1p0_zihintpause2p0_zimop1p0_zce1p0_zcmop1p0_zba1p0_zbb1p0_zbs1p0"},
};

#endif // GET_SUPPORTED_PROFILES

#ifndef PROC
#define PROC(ENUM, NAME, DEFAULT_MARCH, FAST_UNALIGNED_ACCESS)
#endif

PROC(GENERIC_RV32, {"generic-rv32"}, {"rv32i2p1"}, 0)
PROC(GENERIC_RV64, {"generic-rv64"}, {"rv64i2p1"}, 0)
PROC(ROCKET_RV32, {"rocket-rv32"}, {"rv32i2p1_zicsr2p0_zifencei2p0"}, 0)
PROC(ROCKET_RV64, {"rocket-rv64"}, {"rv64i2p1_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E20, {"sifive-e20"}, {"rv32i2p1_m2p0_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E21, {"sifive-e21"}, {"rv32i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E24, {"sifive-e24"}, {"rv32i2p1_m2p0_a2p1_f2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E31, {"sifive-e31"}, {"rv32i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E34, {"sifive-e34"}, {"rv32i2p1_m2p0_a2p1_f2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_E76, {"sifive-e76"}, {"rv32i2p1_m2p0_a2p1_f2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_P450, {"sifive-p450"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zifencei2p0_zihintntl1p0_zihintpause2p0_zihpm2p0_za64rs1p0_zfhmin1p0_zba1p0_zbb1p0_zbs1p0"}, 1)
PROC(SIFIVE_P670, {"sifive-p670"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zic64b1p0_zicbom1p0_zicbop1p0_zicboz1p0_ziccamoa1p0_ziccif1p0_zicclsm1p0_ziccrse1p0_zifencei2p0_zihintntl1p0_zihintpause2p0_zihpm2p0_za64rs1p0_zfhmin1p0_zba1p0_zbb1p0_zbs1p0_zvbb1p0_zvknc1p0_zvkng1p0_zvksc1p0_zvksg1p0_zvl128b1p0"}, 1)
PROC(SIFIVE_S21, {"sifive-s21"}, {"rv64i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_S51, {"sifive-s51"}, {"rv64i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SIFIVE_S54, {"sifive-s54"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_S76, {"sifive-s76"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zifencei2p0_zihintpause2p0"}, 0)
PROC(SIFIVE_U54, {"sifive-u54"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_U74, {"sifive-u74"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zifencei2p0"}, 0)
PROC(SIFIVE_X280, {"sifive-x280"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zifencei2p0_zfh1p0_zba1p0_zbb1p0_zvfh1p0_zvl512b1p0"}, 0)
PROC(SYNTACORE_SCR1_BASE, {"syntacore-scr1-base"}, {"rv32i2p1_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(SYNTACORE_SCR1_MAX, {"syntacore-scr1-max"}, {"rv32i2p1_m2p0_c2p0_zicsr2p0_zifencei2p0"}, 0)
PROC(VENTANA_VEYRON_V1, {"veyron-v1"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicbom1p0_zicbop1p0_zicboz1p0_zicntr2p0_zicsr2p0_zifencei2p0_zihintpause2p0_zihpm2p0_zba1p0_zbb1p0_zbc1p0_zbs1p0_xventanacondops1p0"}, 0)
PROC(XIANGSHAN_NANHU, {"xiangshan-nanhu"}, {"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicbom1p0_zicboz1p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0_zbc1p0_zbs1p0_zkn1p0_zksed1p0_zksh1p0_svinval1p0"}, 0)

#undef PROC

#ifndef TUNE_PROC
#define TUNE_PROC(ENUM, NAME)
#endif

TUNE_PROC(GENERIC, "generic")
TUNE_PROC(ROCKET, "rocket")
TUNE_PROC(SIFIVE_7, "sifive-7-series")

#undef TUNE_PROC
