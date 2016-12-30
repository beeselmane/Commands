#ifndef __CORONA_X__
#define __CORONA_X__ 1

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// Corona-X.h - Standard defines for Corona-X                      //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// beeselmane - 20.8.2015/27.11.2015                               //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

#ifdef __x86_64__
    #define kCXLittleEndian         1
    #define kCXBigEndian            0
    #define kCXEmbedded             0

    #define kCXArchX86_64           1
    #define kCXArchI386             0
    #define kCXArchIA               1
    #define kCXArchARM64            0
    #define kCXArchARMv7            0
    #define kCXArchARM              0
#elif defined(__i386__)
    #define kCXLittleEndian         1
    #define kCXBigEndian            0
    #define kCXEmbedded             0

    #define kCXArchX86_64           0
    #define kCXArchI386             1
    #define kCXArchIA               1
    #define kCXArchARM64            0
    #define kCXArchARMv7            0
    #define kCXArchARM              0
#elif defined(__arm64__) && defined(__ARM_ARCH) && (__ARM_ARCH == 8)
    #define kCXLittleEndian         1
    #define kCXBigEndian            0
    #define kCXEmbedded             1

    #define kCXArchX86_64           0
    #define kCXArchI386             0
    #define kCXArchIA               0
    #define kCXArchARM64            1
    #define kCXArchARMv7            0
    #define kCXArchARM              1
#elif defined(__arm__) && defined(__ARM_ARCH) && (__ARM_ARCH == 7)
    #define kCXLittleEndian         1
    #define kCXBigEndian            0
    #define kCXEmbedded             1

    #define kCXArchX86_64           0
    #define kCXArchI386             0
    #define kCXArchIA               0
    #define kCXArchARM64            0
    #define kCXArchARMv7            1
    #define kCXArchARM              1
#else /* Unsupported */
    #error Unsupported Architecture!
#endif /* Target Architecture */

#if defined(__LP64__) || defined(_LP64)
    #define kCXABI32Bit             0
    #define kCXABI64Bit             1
#else /* LP32 */
    #define kCXABI32Bit             1
    #define kCXABI64Bit             0
#endif /* Target ABI */

#ifdef __CORONA__
    #define kCXTargetOSCorona       1
    #define kCXTargetOSApple        0
    #define kCXTargetOSWindows      0
    #define kCXTargetOSLinux        0
#elif defined(__APPLE__) || defined(__APPLE_CC__)
    #define kCXTargetOSCorona       0
    #define kCXTargetOSApple        1
    #define kCXTargetOSWindows      0
    #define kCXTargetOSLinux        0
#elif defined(_WIN32)
    #define kCXTargetOSCorona       0
    #define kCXTargetOSApple        0
    #define kCXTargetOSWindows      1
    #define kCXTargetOSLinux        0
#elif defined(__linux) || defined(__unix)
    #define kCXTargetOSCorona       0
    #define kCXTargetOSApple        0
    #define kCXTargetOSWindows      0
    #define kCXTargetOSLinux        1
#else /* Unsupported */
    #error Unsupported OS!
#endif /* Target OS */

#ifdef __posix
    #define kCXFullPOSIX            1
#else /* No POSIX */
    #define kCXFullPOSIX            0
#endif /* POSIX Detection */

#ifdef __clang__
    #define kCXCompilerClang        1
    #define kCXCompilerGCC          0
    #define kCXCompilerMSC          0

    #define kCXCompilerName         "Clang"
#elif defined(__gcc__)
    #define kCXCompilerClang        0
    #define kCXCompilerGCC          1
    #define kCXCompilerMSC          0

    #define kCXCompilerName         "GCC"
#elif defined(_MSC_VER)
    #define kCXCompilerClang        0
    #define kCXCompilerGCC          0
    #define kCXCompilerMSC          1

    #define kCXCompilerName         "Microsoft C Compiler"
#else /* Unsupported */
    #error Unsupported Compiler!
#endif /* Compiler */

#define kCXSTDCXXVersionCXX14       201402L
#define kCXSTDCXXVersionCXX11       201103L

#define kCXSTDCVersionC11           201112L

#ifdef __cplusplus
    #define kCXLanguageC            0
    #define kCXLanguageCXX          1
    #define kCXLanguageVersion      __cplusplus

    #define kCXLanguageCXX11        (kCXLanguageVersion >= kCXSTDCXXVersionCXX11)
    #define kCXLanguageCXX14        (kCXLanguageVersion >= kCXSTDCXXVersionCXX14)

    #define kCXLanguageC98          0
    #define kCXLanguageC99          0
    #define kCXLanguageC11          0
#else /* Regular C */
    #define kCXLanguageC            1
    #define kCXLanguageCXX          0
    #define kCXLanguageVersion      __STDC_VERSION__

    #define kCXLanguageCXX11        0
    #define kCXLanguageCXX14        0

    #define kCXLanguageC98          (kCXLanguageVersion >= kCXSTDCVersionC98)
    #define kCXLanguageC99          (kCXLanguageVersion >= kCXSTDCVersionC99)
    #define kCXLanguageC11          (kCXLanguageVersion >= kCXSTDCVersionC11)
#endif /* Language Detection */

#ifdef __OBJC
    #define kCXLanguageObjC         1

    #ifdef __OBJC2
        #define kCXObjC2            1
    #endif /* Objective-C 2 */
#else /* Regular C */
    #define kCXLanguageObjC         0
    #define kCXObjC2                0
#endif /* Objective-C */

// Note: If preprocessing an assembly file,
// compile with -DkCXAssemblyCode=1 to ensure
// that no C definitions are included in the resulting assembly code
#ifndef kCXAssemblyCode
    #define kCXAssemblyCode         0
#endif /* Assembly Code */

#ifdef __has_feature
    #define CXHasFeature            __has_feature
#else /* !defined(__has_feature) */
    #define CXHasFeature(...)       0
#endif /* Compiler Feature Detection */

#ifdef __has_attribute
    #define CXHasAttribue           __has_attribute
#else /* !defined(__has_attribute) */
    #define CXHasAttribue(...)      0
#endif /* Compiler Attribute Detection */

#ifdef __has_cpp_attribute
    #define CXHasCXXAttribute       __has_cpp_attribute
#else /* !defined(__has_cpp_attribute) */
    #define CXHasCXXAttribute(...)  0
#endif /* Compiler C++ Attribute Detection */

#ifdef __has_extension
    #define CXHasExtension          __has_extension
#else /* !defined(__has_extension) */
    #define CXHasExtension(e)       CXHasFeature(e)
#endif /* Compiler Extension Detection */

#ifdef __has_builtin
    #define CXHasBuiltin            __has_builtin
#else /* !defined(__has_builtin) */
    #define CXHasBuiltIn(...)       0
#endif /* Compiler BuiltIn Detection */

#ifdef __has_include
    #define CXHasInclude            __has_include
#else  /* !defined(__has_include) */
    #define CXHasInclude(...)       0
#endif /* Include file checking */

#ifdef __has_include_next
    #define CXHasIncludeNext        __has_include_next
#else  /* !defined(__has_include_next) */
    #define CXHasIncludeNext(...)   0
#endif /* Include file checking */

#ifdef __BASE_FILE__
    #define kCXBaseFile             __BASE_FILE__
#else /* !defined(__BASE_FILE__) */
    #define kCXBaseFile             "Unknown"
#endif /* Base File Name */

#ifdef __TIMESTAMP__
    #define kCXTimestamp            __TIMESTAMP__
#else /* !defined(__TIMESTAMP__) */
    #define __TIMESTAMP__           "Unknown"
#endif /* Current file timestamp */

#ifdef __clang_version__
    #define kCXCompilerVersion      __clang_version__
#else /* !defined(__clang_version) */
    #define kCXCompilerVersion      "Unknown"
#endif /* Clang Version */

#define kCXHasVectorExtension               CXHasExtension(attribute_ext_vector_type)
#define kCXHasEnumeratorAttributes          CXHasExtension(enumerator_attributes)
#define kCXHasCXXExceptions                 CXHasFeature(cxx_exceptions)
#define kCXHasCXXRTTI                       CXHasFeature(cxx_rtti)
#define kCXHasCXXAccessSFINAEErrors         CXHasFeature(cxx_access_control_sfinae)
#define kCXHasCXXAliasTemplates             CXHasFeature(cxx_alias_templates)
#define kCXHasCXXAlignAs                    CXHasFeature(cxx_alignas)
#define kCXHasCXXAlignOf                    CXHasFeature(cxx_alignof)
#define kCXHasCXXAttributes                 CXHasFeature(cxx_attributes)
#define kCXHasCXXConstExpr                  CXHasFeature(cxx_constexpr)
#define kCXHasCXXDeclType                   CXHasFeature(cxx_decltype)
#define kCXHasCXXDeclTypeIncompleteReturn   CXHasFeature(cxx_decltype_incomplete_return_types)
#define kCXHasCXXTemplateDefaultArgs        CXHasFeature(cxx_default_function_template_args)
#define kCXHasCXXDefaultedFunctions         CXHasFeature(cxx_defaulted_functions)
#define kCXHasCXXDelegatedConstructors      CXHasFeature(cxx_delegating_constructors)
#define kCXHasCXXDeletedFunctions           CXHasFeature(cxx_deleted_functions)
#define kCXHasCXXExplicitConversions        CXHasFeature(cxx_explicit_conversions)
#define kCXHasCXXGeneralizedInitializers    CXHasFeature(cxx_generalized_initializers)
#define kCXHasCXXImplicitMoves              CXHasFeature(cxx_implicit_moves)
#define kCXHasCXXStronglyTypedEnumerators   CXHasFeature(cxx_strong_enums)

#define kCXHasCAlignAs                      CXHasFeature(c_alignas)
#define kCXHasCAlignOf                      CXHasFeature(c_alignof)
#define kCXHasCAtomic                       CXHasFeature(c_atomic)
#define kCXHasCGenericSelections            CXHasFeature(c_generic_selections)
#define kCXHasCStaticAssert                 CXHasFeature(c_static_assert)
#define kCXHasCThreadLocal                  CXHasFeature(c_thread_local)

#define kCXHasObjCInstancetype              CXHasFeature(objc_instancetype)
#define kCXHasObjCARC                       CXHasFeature(objc_arc)
#define kCXHasObjCARCWeakValues             CXHasFeature(objc_arc_weak)
#define kCXHasObjCFixedEnums                CXHasFeature(objc_fixed_enum)
#define kCXHasObjCArrayLiterals             CXHasFeature(objc_array_literals)
#define kCXHasObjCDictionaryLiterals        CXHasFeature(objc_dictionary_literals)
#define kCXHasObjCSubscripting              CXHasFeature(objc_subscripting)
#define kCXHasObjCAutosynthesis             CXHasFeature(objc_default_synthesize_properties)

#define kCXHasModules                       CXHasFeature(modules)
#define kCXHasBlocks                        CXHasFeature(blocks)

/// TODO: Define Availability Macros

#define CXPackStruct                        __attribute__((packed))
#define CXAlignStruct(n)                    __attribute__((aligned(n)))

#endif /* !defined(__CORONA_X__) */
