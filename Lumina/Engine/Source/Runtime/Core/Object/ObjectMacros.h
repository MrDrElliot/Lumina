#pragma once

#include "Memory/Memory.h"
#include "Core/Object/ObjectCore.h"


class Lumina::CClass;

#define NO_API



#define LUM_CLASS(...)
#define LUM_STRUCT(...)
#define LUM_ENUM(...)
#define LUM_FIELD(...)
#define LUM_FUNCTION(...)


#define MACRO_BODY_COMBINE(A, B, C, D) A##B##C##D
#define MACRO_BODY_COMBINE_EXPAND(A, B, C, D) MACRO_BODY_COMBINE(A, B, C, D)


// =====================================================================================================
// NOTE: Clang has a known limitation where it struggles to properly expand variadic macros
// during parsing (especially when using libclang or Clang tooling).
// 
// When REFLECTION_PARSER is defined (i.e., when running our custom reflection parser),
// we disable the GENERATED_BODY macro expansion to prevent parsing errors and allow
// correct type discovery.
//
// This workaround ensures that Clang sees an empty GENERATED_BODY(...) instead of trying
// to expand it and breaking the AST.
//
// IMPORTANT: This does not affect normal compilation; only parsing for reflection!
// =====================================================================================================
#if defined(REFLECTION_PARSER)
#define GENERATED_BODY(...)
#else
#define GENERATED_BODY(...) MACRO_BODY_COMBINE_EXPAND(CURRENT_FILE_ID, _, __LINE__, _GENERATED_BODY)
#endif




#define DECLARE_CLASS(TClass, TBaseClass, TAPI) \
private: \
    TClass& operator=(TClass&&); \
    TClass& operator=(const TClass&); \
    TAPI static CClass* GetPrivateStaticClass(); \
public: \
    /** Simple helper typedefs */ \
    using ThisClass = TClass; \
    using Super = TBaseClass; \
    /** Returns CClass object representing this class */ \
    inline static CClass* StaticClass() \
    { \
        return GetPrivateStaticClass(); \
    } \


#define IMPLEMENT_CLASS(TClass) \
    FClassRegistrationInfo Registration_Info_CClass_##TClass; \
    CClass* TClass::GetPrivateStaticClass() \
    { \
        if (Registration_Info_CClass_##TClass.Singleton == nullptr) \
        { \
            AllocateStaticClass(TEXT(#TClass), &Registration_Info_CClass_##TClass.Singleton, sizeof(TClass), alignof(TClass)); \
        } \
        return Registration_Info_CClass_##TClass.Singleton; \
    }



#define BEGIN_DATA_DESC(TClass) static struct Z_DataDesc_##TClass {

#define DEFINE_DATA_FIELD(Field)

#define END_DATA_DESC(TClass) }
