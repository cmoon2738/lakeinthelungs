#ifndef _LAKE_common_h_
#define _LAKE_common_h_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define LAKE_VERSION_MAJOR 0
#define LAKE_VERSION_MINOR 1
#define LAKE_VERSION_REVISION 1

#define VERSION_NUM(major, minor, revision) \
    ((major) * 1000000 + (minor) * 1000 + (revision))

#define LAKE_VERSION VERSION_NUM(LAKE_VERSION_MAJOR, LAKE_VERSION_MINOR, LAKE_VERSION_REVISION)

#define VERSION_MAJOR(version) ((version) / 1000000)
#define VERSION_MINOR(version) (((version) / 1000) % 1000)
#define VERSION_REVISION(version) ((version) % 1000)

#if !defined(LAKE_NDEBUG) && !defined(LAKE_DEBUG)
    #define LAKE_NDEBUG 1
#endif

#ifndef LAKE_PLATFORM_WINDOWS
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
        #define LAKE_PLATFORM_WINDOWS 1
        #ifndef VK_USE_PLATFORM_WIN32_KHR
            #define VK_USE_PLATFORM_WIN32_KHR
        #endif
    #endif
#endif
#ifndef LAKE_PLATFORM_UNIX
    #if defined(__unix__) || defined(__unix) || defined(unix)
        #define LAKE_PLATFORM_UNIX 1
    #endif
#endif
#ifndef LAKE_PLATFORM_APPLE
    #if defined(__APPLE__)
        #define LAKE_PLATFORM_APPLE 1
        #ifndef VK_USE_PLATFORM_METAL_EXT
            #define VK_USE_PLATFORM_METAL_EXT    /* both Mac & iOS */
        #endif
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            #ifndef LAKE_PLATFORM_IOS
                #define LAKE_PLATFORM_IOS 1
            #endif
        #else
            #define LAKE_PLATFORM_MACOSX 1
        #endif
    #endif
#endif
#ifndef LAKE_PLATFORM_ANDROID
    #if defined(__ANDROID__)
        #define LAKE_PLATFORM_ANDROID 1
        #ifndef VK_USE_PLATFORM_ANDROID_KHR
            #define VK_USE_PLATFORM_ANDROID_KHR
        #endif
    #endif
#endif
#ifndef LAKE_PLATFORM_EMSCRIPTEN
    #if defined(__EMSCRIPTEN__)
        #define LAKE_PLATFORM_EMSCRIPTEN 1
    #endif
#endif 
#ifndef LAKE_PLATFORM_LINUX
    #if defined(__linux__) || defined(__gnu_linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
        #define LAKE_PLATFORM_LINUX 1
        #if defined(LAKE_NATIVE_WAYLAND)
            #ifndef VK_USE_PLATFORM_WAYLAND_KHR
                #define VK_USE_PLATFORM_WAYLAND_KHR
            #endif
        #endif
        #if defined(LAKE_NATIVE_XCB)
            #ifndef VK_USE_PLATFORM_XCB_KHR
                #define VK_USE_PLATFORM_XCB_KHR
            #endif
        #endif
        #if defined(LAKE_NATIVE_KMS)
            #ifndef VK_USE_PLATFORM_KMS_KHR
                #define VK_USE_PLATFORM_KMS_KHR
            #endif
        #endif
    #endif
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC__MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(CC_GNUC_VERSION)
    #define CC_GNUC_VERSION_CHECK(ma,mi,rev) (CC_GNUC_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_GNUC_VERSION_NOT(ma,mi,rev) (CC_GNUC_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_GNUC_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_GNUC_VERSION_NOT(ma,mi,rev) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(CC_MSVC_VERSION)
    #define CC_MSVC_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_MSVC_VERSION_NOT(ma,mi,rev) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define CC_MSVC_VERSION_CHECK(ma,mi,rev) (_MSC_FULL_VER >= ((ma * 10000000) + (mi * 100000) + (rev)))
    #define CC_MSVC_VERSION_NOT(ma,mi,rev) (_MSC_FULL_VER < ((ma * 10000000) + (mi * 100000) + (rev)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define CC_MSVC_VERSION_CHECK(ma,mi,rev) (_MSC_FULL_VER >= ((ma * 1000000) + (mi * 10000) + (rev)))
    #define CC_MSVC_VERSION_NOT(ma,mi,rev) (_MSC_FULL_VER < ((ma * 1000000) + (mi * 10000) + (rev)))
#else
    #define CC_MSVC_VERSION_CHECK(ma,mi,rev) (_MSC_VER >= ((ma * 100) + (mi)))
    #define CC_MSVC_VERSION_NOT(ma,mi,rev) (_MSC_VER < ((ma * 100) + (mi)))
#endif

/* INTEL */
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && !defined(__ICL)
    #define CC_INTEL_VERSION VERSION_NUM(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#elif defined(__INTEL_COMPILER) && !defined(__ICL)
    #define CC_INTEL_VERSION VERSION_NUM(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#endif
#if defined(CC_INTEL_VERSION)
    #define CC_INTEL_VERSION_CHECK(ma,mi,rev) (CC_INTEL_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_INTEL_VERSION_NOT(ma,mi,rev) (CC_INTEL_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_INTEL_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_INTEL_VERSION_NOT(ma,mi,rev) (0)
#endif

/* PGI */
#if defined(__PGI) && defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__)
    #define CC_PGI_VERSION VERSION_NUM(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#endif
#if defined(CC_PGI_VERSION)
    #define CC_PGI_VERSION_CHECK(ma,mi,rev) (CC_PGI_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_PGI_VERSION_NOT(ma,mi,rev) (CC_PGI_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_PGI_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_PGI_VERSION_NOT(ma,mi,rev) (0)
#endif

/* SUNPRO */
#if defined(__SUNPRO_C) && (__SUNPRO_C > 0x1000)
    #define CC_SUNPRO_VERSION VERSION_NUM((((__SUNPRO_C >> 16) & 0xf) * 10) + ((__SUNPRO_C >> 12) & 0xf), \
            (((__SUNPRO_C >> 8) & 0xf) * 10) + ((__SUNPRO_C >> 4) & 0xf), (__SUNPRO_C & 0xf) * 10)
#elif defined(__SUNPRO_C)
    #define CC_SUNPRO_VERSION VERSION_NUM((__SUNPRO_C >> 8) & 0xf, (__SUNPRO_C >> 4) & 0xf, (__SUNPRO_C) & 0xf)
#elif defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x1000)
    #define CC_SUNPRO_VERSION VERSION_NUM((((__SUNPRO_CC >> 16) & 0xf) * 10) + ((__SUNPRO_CC >> 12) & 0xf), \
            (((__SUNPRO_CC >> 8) & 0xf) * 10) + ((__SUNPRO_CC >> 4) & 0xf), (__SUNPRO_CC & 0xf) * 10)
#elif defined(__SUNPRO_CC)
    #define CC_SUNPRO_VERSION VERSION_NUM((__SUNPRO_CC >> 8) & 0xf, (__SUNPRO_CC >> 4) & 0xf, (__SUNPRO_CC) & 0xf)
#endif
#if defined(CC_SUNPRO_VERSION)
    #define CC_SUNPRO_VERSION_CHECK(ma,mi,rev) (CC_SUNPRO_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_SUNPRO_VERSION_NOT(ma,mi,rev) (CC_SUNPRO_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_SUNPRO_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_SUNPRO_VERSION_NOT(ma,mi,rev) (0)
#endif

/* EMSCRIPTEN */
#if defined(CC_PLATFORM_EMSCRIPTEN) || defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #define CC_EMSCRIPTEN_VERSION VERSION_NUM(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif
#if defined(CC_EMSCRIPTEN_VERSION)
    #define CC_EMSCRIPTEN_VERSION_CHECK(ma,mi,rev) (CC_EMSCRIPTEN_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_EMSCRIPTEN_VERSION_NOT(ma,mi,rev) (CC_EMSCRIPTEN_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_EMSCRIPTEN_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_EMSCRIPTEN_VERSION_NOT(ma,mi,rev) (0)
#endif

/* ARM */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, \
            (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCC_VERSION / 1000000, \
            (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(CC_ARM_VERSION)
    #define CC_ARM_VERSION_CHECK(ma,mi,rev) (CC_ARM_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_ARM_VERSION_NOT(ma,mi,rev) (CC_ARM_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_ARM_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_ARM_VERSION_NOT(ma,mi,rev) (0)
#endif

/* IBM */
#if defined(__ibmxl__)
    #define CC_IBM_VERSION VERSION_NUM(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined(__xlC__) && defined(__xlC_ver__)
    #define CC_IBM_VERSION VERSION_NUM(__xlC__ >> 8, __xlC__ & 0xff, (__xlC_ver__ >> 8) & 0xff)
#elif defined(__xlC__)
    #define CC_IBM_VERSION VERSION_NUM(__xlC__ >> 8, __xlC__ & 0xff, 0)
#endif
#if defined(CC_IBM_VERSION)
    #define CC_IBM_VERSION_CHECK(ma,mi,rev) (CC_IBM_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_IBM_VERSION_NOT(ma,mi,rev) (CC_IBM_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_IBM_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_IBM_VERSION_NOT(ma,mi,rev) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(CC_CLANG_VERSION)
    #if __has_warning("-Wmissing-designated-field-initializers")
        #define CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define CC_CLANG_VERSION 100000
    #elif defined(__FILE_NAME__)
        #define CC_CLANG_VERSION 90000
    #elif __has_warning("-Wextra-semi-stmt") || __has_builtin(__builtin_rotateleft32)
        #define CC_CLANG_VERSION 80000
    /* For reasons unknown, Xcode 10.3 (Apple LLVM version 10.0.1) is apparently
     * based on Clang 7, but does not support the warning we test.
     * See https://en.wikipedia.org/wiki/Xcode#Toolchain_versions and
     * https://trac.macports.org/wiki/XcodeVersionInfo. */
    #elif __has_warning("-Wc++98-compat-extra-semi") || \
        (defined(__apple_build_version__) && __apple_build_version__ >= 10010000)
        #define CC_CLANG_VERSION 70000
    #elif __has_warning("-Wpragma-pack")
        #define CC_CLANG_VERSION 60000
    #elif __has_warning("-Wbitfield-enum-conversion")
        #define CC_CLANG_VERSION 50000
    #elif __has_attribute(diagnose_if)
        #define CC_CLANG_VERSION 40000
    #elif __has_warning("-Wcomma")
        #define CC_CLANG_VERSION 39000
    #elif __has_warning("-Wdouble-promotion")
        #define CC_CLANG_VERSION 38000
    #elif __has_warning("-Wshift-negative-value")
        #define CC_CLANG_VERSION 37000
    #elif __has_warning("-Wambiguous-ellipsis")
        #define CC_CLANG_VERSION 36000
    #else
        #define CC_CLANG_VERSION 1
    #endif
#endif
#if defined(CC_CLANG_VERSION)
    #define CC_CLANG_VERSION_CHECK(ma,mi,rev) (CC_CLANG_VERSION >= ((ma * 10000) + (mi * 1000) + (rev)))
    #define CC_CLANG_VERSION_NOT(ma,mi,rev) (CC_CLANG_VERSION < ((ma * 10000) + (mi * 1000) + (rev)))
#else
    #define CC_CLANG_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_CLANG_VERSION_NOT(ma,mi,rev) (0)
#endif

/* GCC */
#if defined(CC_GNUC_VERSION) && \
    !defined(CC_CLANG_VERSION) && \
    !defined(CC_INTEL_VERSION) && \
    !defined(CC_PGI_VERSION) && \
    !defined(CC_ARM_VERSION) && \
    !defined(CC_IBM_VERSION) && \
    !defined(__COMPCERT__)
    #define CC_GCC_VERSION CC_GNUC_VERSION
#endif
#if defined(CC_GCC_VERSION)
    #define CC_GCC_VERSION_CHECK(ma,mi,rev) (CC_GCC_VERSION >= VERSION_NUM(ma,mi,rev))
    #define CC_GCC_VERSION_NOT(ma,mi,rev) (CC_GCC_VERSION < VERSION_NUM(ma,mi,rev))
#else
    #define CC_GCC_VERSION_CHECK(ma,mi,rev) (0)
    #define CC_GCC_VERSION_NOT(ma,mi,rev) (0)
#endif

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    CC_CLANG_VERSION_CHECK(1,0,0) || \
    CC_GCC_VERSION_CHECK(3,0,0) || \
    CC_INTEL_VERSION_CHECK(13,0,0) || \
    CC_PGI_VERSION_CHECK(18,4,0) || \
    CC_SUNPRO_VERSION_CHECK(8,0,0) || \
    CC_ARM_VERSION_CHECK(4,1,0) || \
    (CC_IBM_VERSION_CHECK(10,1,0) && defined(__C99_PRAGMA_OPERATOR))
    #define PRAGMA(x) __Pragma(#x)
#elif CC_MSVC_VERSION_CHECK(15,0,0)
    #define PRAGMA(x) __pragma(x)
#else
    #define PRAGMA(x)
#endif

#if defined(CC_CLANG_VERSION)
    #define DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
    #define DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif CC_INTEL_VERSION_CHECK(13,0,0)
    #define DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define DIAGNOSTIC_POP _Pragma("warning(pop)")
#elif CC_GCC_VERSION_CHECK(4,6,0)
    #define DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
    #define DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif CC_MSVC_VERSION_CHECK(15,0,0)
    #define DIAGNOSTIC_PUSH __pragma(warning(push))
    #define DIAGNOSTIC_POP __pragma(warning(pop))
#elif CC_ARM_VERSION_CHECK(5,6,0)
    #define DIAGNOSTIC_PUSH _Pragma("push")
    #define DIAGNOSTIC_POP _Pragma("pop")
#else
    #define DIAGNOSTIC_PUSH
    #define DIAGNOSTIC_POP
#endif

#if !defined(INLINE) && !defined(NOINLINE)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define INLINE __attribute__((always_inline)) inline
        #define NOINLINE __attribute__((noinline))
    #elif defined(CC_MSVC_VERSION)
        #define INLINE __forceinline
        #define NOINLINE __declspec(noinline)
    #else
        #define INLINE static inline
        #define NOINLINE
    #endif
#endif

#ifndef NORETURN
    #if defined(CC_GCC_VERSION)
        #define NORETURN __attribute__((noreturn))
    #elif defined(CC_MSVC_VERSION)
        #define NORETURN __declspec(noreturn)
    #else
        #define NORETURN
    #endif
#endif

#ifndef UNUSED
    #ifdef CC_GNUC_VERSION
        #define UNUSED __attribute((unused))
    #else
        #define UNUSED
    #endif
#endif

#ifndef __MACH__
    #ifndef NULL
        #ifdef __cplusplus
            #define NULL (0)
        #else
            #define NULL ((void *)0)
        #endif
    #endif
#endif /* ! macOS - breaks precompiled headers */

#ifdef __has_builtin
    #define HAS_BUILTIN(x) __has_builtin(x)
#else
    #define HAS_BUILTIN(x) (0)
#endif

#ifdef __has_feature
    #define HAS_FEATURE(x) __has_feature(x)
#else
    #define HAS_FEATURE(x) (0)
#endif

#ifdef __has_warning
    #define HAS_WARNING(x) __has_warning(x)
#else
    #define HAS_WARNING(x) (0)
#endif

#ifdef CC_GNUC_VERSION
    #define PRINTF_FORMAT(fmtargnumber) __attribute__((format(__printf__, fmtargnumber, fmtargnumber + 1)))
#else
    #define PRINTF_FORMAT(fmtargnumber)
#endif

#if defined(CC_CLANG_VERSION) && defined(__has_attribute)
    #if __has_attribute(target)
        #define HAS_TARGET_ATTRIBS 1
    #endif
#elif CC_GNUC_VERSION_CHECK(4,9,0)
    #define HAS_TARGET_ATTRIBS 1
#elif defined(__ICC) && __ICC >= 1600
    #define HAS_TARGET_ATTRIBS 1
#endif

#ifdef HAS_TARGET_ATTRIBS
    #define TARGETING(x) __attribute__((target(x)))
#else
    #define TARGETING(x)
#endif

/* Alpha
   <https://en.wikipedia.org/wiki/DEC_Alpha> */
#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
    #if defined(__alpha_ev6__)
        #define ARCH_ALPHA 6
    #elif defined(__alpha_ev5__)
        #define ARCH_ALPHA 5
    #elif defined(__alpha_ev4__)
        #define ARCH_ALPHA 4
    #else
        #define ARCH_ALPHA 1
    #endif
#endif
#if defined(ARCH_ALPHA)
    #define ARCH_ALPHA_CHECK(ver) ((ver) <= ARCH_ALPHA)
#else
    #define ARCH_ALPHA_CHECK(ver) (0)
#endif

/* Atmel AVR
   <https://en.wikipedia.org/wiki/Atmel_AVR> */
#if defined(__AVR_ARCH__)
    #define ARCH_AVR __AVR_ARCH__
#endif

/* AMD64 / x86_64
   <https://en.wikipedia.org/wiki/X86-64> */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #if !defined(_M_ARM64EC)
        #define ARCH_AMD64 1000
    #endif
#endif

/* ARM
   <https://en.wikipedia.org/wiki/ARM_architecture> */
#if defined(__ARM_ARCH)
    #if __ARM_ARCH > 100
        #define ARCH_ARM (__ARM_ARCH)
    #else
        #define ARCH_ARM (__ARM_ARCH * 100)
    #endif
#elif defined(_M_ARM)
    #if _M_ARM > 100
        #define ARCH_ARM (_M_ARM)
    #else
        #define ARCH_ARM (_M_ARM * 100)
    #endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
    #define ARCH_ARM 800
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARM)
    #define ARCH_ARM 1
    #if defined(CC_MSVC_VERSION)
        #include <armintr.h>
    #endif
#endif
#if defined(ARCH_ARM)
    #define ARCH_ARM_CHECK(ma,mi) (((ma * 100) + (mi)) <= ARCH_ARM)
#else
    #define ARCH_ARM_CHECK(ma,mi) (0)
#endif

/* AArch64
   <https://en.wikipedia.org/wiki/ARM_architecture> */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define ARCH_AARCH64 1000
    #if defined(CC_MSVC_VERSION)
        #include <arm64intr.h>
    #endif
#endif
#if defined(ARCH_AARCH64)
    #define ARCH_AARCH64_CHECK(ver) ((ver) <= ARCH_AARCH64)
#else
    #define ARCH_AARCH64_CHECK(ver) (0)
#endif

/* ARM SIMD ISA extensions */
#if defined(__ARM_NEON) || defined(ARCH_AARCH64)
    #if defined(ARCH_AARCH64)
        #define ARCH_ARM_NEON ARCH_AARCH64
        #include <arm64_neon.h>
    #elif defined(ARCH_ARM)
        #define ARCH_ARM_NEON ARCH_ARM
        #include <arm_neon.h>
    #endif
#endif
#if defined(__ARM_FEATURE_AES) && __ARM_FEATURE_AES
    #define ARCH_ARM_AES 1
#endif
#if defined(__ARM_FEATURE_COMPLEX) && __ARM_FEATURE_COMPLEX
    #define ARCH_ARM_COMPLEX 1
#endif
#if defined(__ARM_FEATURE_CRYPTO) && __ARM_FEATURE_CRYPTO
    #define ARCH_ARM_CRYPTO 1
#endif
#if defined(__ARM_FEATURE_CRC32) && __ARM_FEATURE_CRC32
    #define ARCH_ARM_CRC32 1
#endif
#if defined(__ARM_FEATURE_DOTPROD) && __ARM_FEATURE_DOTPROD
    #define ARCH_ARM_DOTPROD 1
#endif
#if defined(__ARM_FEATURE_FMA) && __ARM_FEATURE_FMA
    #define ARCH_ARM_FMA 1
#endif
#if defined(__ARM_FEATURE_FP16_FML) && __ARM_FEATURE_FP16_FML
    #define ARCH_ARM_FP16_FML 1
#endif
#if defined(__ARM_FEATURE_FRINT) && __ARM_FEATURE_FRINT
    #define ARCH_ARM_FRINT 1
#endif
#if defined(__ARM_FEATURE_MATMUL_INT8) && __ARM_FEATURE_MATMUL_INT8
    #define ARCH_ARM_MATMUL_INT8 1
#endif
#if defined(__ARM_FEATURE_SHA2) && __ARM_FEATURE_SHA2 && !defined(__APPLE_CC__)
    #define ARCH_ARM_SHA2 1
#endif
#if defined(__ARM_FEATURE_SHA3) && __ARM_FEATURE_SHA3
    #define ARCH_ARM_SHA3 1
#endif
#if defined(__ARM_FEATURE_SHA512) && __ARM_FEATURE_SHA512
    #define ARCH_ARM_SHA512 1
#endif
#if defined(__ARM_FEATURE_SM3) && __ARM_FEATURE_SM3
    #define ARCH_ARM_SM3 1
#endif
#if defined(__ARM_FEATURE_SM4) && __ARM_FEATURE_SM4
    #define ARCH_ARM_SM4 1
#endif
#if defined(__ARM_FEATURE_SVE) && __ARM_FEATURE_SVE
    #define ARCH_ARM_SVE 1
#endif
#if defined(__ARM_FEATURE_QRDMX) && __ARM_FEATURE_QRDMX
    #define ARCH_ARM_QRDMX 1
#endif

/* Blackfin
   <https://en.wikipedia.org/wiki/Blackfin> */
#if defined(__bfin) || defined(__BFIN__) || defined(__bfin__)
    #define ARCH_BLACKFIN 1
#endif

/* CRIS
   <https://en.wikipedia.org/wiki/ETRAX_CRIS> */
#if defined(__CRIS_arch_version)
    #define ARCH_CRIS __CRIS_arch_version
#elif defined(__cris__) || defined(__cris) || defined(__CRIS) || defined(__CRIS__)
    #define ARCH_CRIS 1
#endif

/* Convex
   <https://en.wikipedia.org/wiki/Convex_Computer> */
#if defined(__convex_c38__)
    #define ARCH_CONVEX 38
#elif defined(__convex_c34__)
    #define ARCH_CONVEX 34
#elif defined(__convex_c32__)
    #define ARCH_CONVEX 32
#elif defined(__convex_c2__)
    #define ARCH_CONVEX 2
#elif defined(__convex__)
    #define ARCH_CONVEX 1
#endif
#if defined(ARCH_CONVEX)
    #define ARCH_CONVEX_CHECK(version) ((version) <= ARCH_CONVEX)
#else
    #define ARCH_CONVEX_CHECK(version) (0)
#endif

/* Adapteva Epiphany
   <https://en.wikipedia.org/wiki/Adapteva_Epiphany> */
#if defined(__epiphany__)
    #define ARCH_EPIPHANY 1
#endif

/* Fujitsu FR-V
   <https://en.wikipedia.org/wiki/FR-V_(microprocessor)> */
#if defined(__frv__)
    #define ARCH_FRV 1
#endif

/* H8/300
   <https://en.wikipedia.org/wiki/H8_Family> */
#if defined(__H8300__)
    #define ARCH_H8300 1
#endif

/* Elbrus (8S, 8SV and successors)
   <https://en.wikipedia.org/wiki/Elbrus-8S> */
#if defined(__e2k__)
    #define ARCH_E2K 1
#endif

/* HP/PA / PA-RISC
   <https://en.wikipedia.org/wiki/PA-RISC> */
#if defined(__PA8000__) || defined(__HPPA20__) || defined(__RISC2_0__) || defined(_PA_RISC2_0)
    #define ARCH_HPPA 20
#elif defined(__PA7100__) || defined(__HPPA11__) || defined(_PA_RISC1_1)
    #define ARCH_HPPA 11
#elif defined(_PA_RISC1_0)
    #define ARCH_HPPA 10
#elif defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
    #define ARCH_HPPA 1
#endif
#if defined(ARCH_HPPA)
    #define ARCH_HPPA_CHECK(version) ((version) <= ARCH_HPPA)
#else
    #define ARCH_HPPA_CHECK(version) (0)
#endif

/* x86
   <https://en.wikipedia.org/wiki/X86> */
#if defined(_M_IX86)
    #define ARCH_X86 (_M_IX86 / 100)
#elif defined(__I86__)
    #define ARCH_X86 __I86__
#elif defined(i686) || defined(__i686) || defined(__i686__)
    #define ARCH_X86 6
#elif defined(i586) || defined(__i586) || defined(__i586__)
    #define ARCH_X86 5
#elif defined(i486) || defined(__i486) || defined(__i486__)
    #define ARCH_X86 4
#elif defined(i386) || defined(__i386) || defined(__i386__)
    #define ARCH_X86 3
#elif defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__)
    #define ARCH_X86 3
#endif
#if defined(ARCH_X86)
    #define ARCH_X86_CHECK(version) ((version) <= ARCH_X86)
#else
    #define ARCH_X86_CHECK(version) (0)
#endif

/* SIMD ISA extensions for x86/x86_64 and Elbrus */
#if defined(ARCH_X86) || defined(ARCH_AMD64) || defined(ARCH_E2K)
    #if defined(_M_IX86_FP)
        #define ARCH_X86_MMX 1
        #include <mmintrin.h>
        #if (_M_IX86_FP >= 1)
            #define ARCH_X86_SSE 1
            #include <xmmintrin.h>
        #endif
        #if (_M_IX86_FP >= 2)
            #define ARCH_X86_SSE 1
            #define ARCH_X86_SSE2 1
            #include <emmintrin.h>
        #endif
    #elif defined(_M_X64)
        #define ARCH_X86_SSE 1
        #define ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #else
        #if defined(__MMX__)
            #define ARCH_X86_MMX 1
            #include <mmintrin.h>
        #endif
        #if defined(__SSE__)
            #define ARCH_X86_SSE 1
            #include <xmmintrin.h>
        #endif
        #if defined(__SSE2__)
            #define ARCH_X86_SSE2 1
            #include <emmintrin.h>
        #endif
    #endif
    #if defined(__SSE3__)
        #define ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if defined(__SSSE3__)
        #define ARCH_X86_SSSE3 1
        #include <tmmintrin.h>
    #endif
    #if defined(__SSE4_1__)
        #define ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if defined(__SSE4_2__)
        #define ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #if defined(__SSE4A__)
        #define ARCH_X86_SSE4A 1
        #include <ammintrin.h>
    #endif
    #if defined(__XOP__)
        #define ARCH_X86_XOP 1
        #include <xopintrin.h>
    #endif
    #if defined(__AVX__)
        #define ARCH_X86_AVX 1
        #if !defined(ARCH_X86_SSE3)
            #define ARCH_X86_SSE3 1
            #include <pmmintrin.h>
        #endif
        #if !defined(ARCH_X86_SSE4_1)
            #define ARCH_X86_SSE4_1 1
            #include <smmintrin.h>
        #endif
        #if !defined(ARCH_X86_SSE4_2)
            #define ARCH_X86_SSE4_2 1
            #include <nmmintrin.h>
        #endif
        #include <immintrin.h>
    #endif
    #if defined(__AVX2__)
        #define ARCH_X86_AVX2 1
        #if defined(CC_MSVC_VERSION)
            #define ARCH_X86_FMA 1
        #endif
        #include <immintrin.h>
    #endif
    #if defined(__FMA__)
        #define ARCH_X86_FMA 1
        #if !defined(ARCH_X86_AVX)
            #define ARCH_X86_AVX 1
        #endif
        #include <immintrin.h>
    #endif
    #if defined(__AVX512VP2INTERSECT__)
        #define ARCH_X86_AVX512VP2INTERSECT 1
    #endif
    #if defined(__AVX512BITALG__)
        #define ARCH_X86_AVX512BITALG 1
    #endif
    #if defined(__AVX512VPOPCNTDQ__)
        #define ARCH_X86_AVX512VPOPCNTDQ 1
    #endif
    #if defined(__AVX512VBMI__)
        #define ARCH_X86_AVX512VBMI 1
    #endif
    #if defined(__AVX512VBMI2__)
        #define ARCH_X86_AVX512VBMI2 1
    #endif
    #if defined(__AVX512VNNI__)
        #define ARCH_X86_AVX512VNNI 1
    #endif
    #if defined(__AVX5124VNNIW__)
        #define ARCH_X86_AVX5124VNNIW 1
    #endif
    #if defined(__AVX512BW__)
        #define ARCH_X86_AVX512BW 1
    #endif
    #if defined(__AVX512BF16__)
        #define ARCH_X86_AVX512BF16 1
    #endif
    #if defined(__AVX512CD__)
        #define ARCH_X86_AVX512CD 1
    #endif
    #if defined(__AVX512DQ__)
        #define ARCH_X86_AVX512DQ 1
    #endif
    #if defined(__AVX512F__)
        #define ARCH_X86_AVX512F 1
    #endif
    #if defined(__AVX512VL__)
        #define ARCH_X86_AVX512VL 1
    #endif
    #if defined(__AVX512FP16__)
        #define ARCH_X86_AVX512FP16 1
    #endif
    #if defined(__GFNI__)
        #define ARCH_X86_GFNI 1
    #endif
    #if defined(__PCLMUL__)
        #define ARCH_X86_PCLMUL 1
    #endif
    #if defined(__VPCLMULQDQ__)
        #define ARCH_X86_VPCLMULQDQ 1
    #endif
    #if (defined(__F16C__) || CC_MSVC_VERSION_CHECK(19,30,0)) && defined(ARCH_X86_AVX2)
        #define ARCH_X86_F16C 1
    #endif
    #if defined(__AES__)
        #define ARCH_X86_AES 1
    #endif
#endif

/* Itanium
   <https://en.wikipedia.org/wiki/Itanium> */
#if defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) || defined(__itanium__)
    #define ARCH_IA64 1
#endif

/* Renesas M32R
   <https://en.wikipedia.org/wiki/M32R> */
#if defined(__m32r__) || defined(__M32R__)
    #define ARCH_M32R 1
#endif

/* Motorola 68000
   <https://en.wikipedia.org/wiki/Motorola_68000> */
#if defined(__mc68060__) || defined(__MC68060__)
    #define ARCH_M68K 68060
#elif defined(__mc68040__) || defined(__MC68040__)
    #define ARCH_M68K 68040
#elif defined(__mc68030__) || defined(__MC68030__)
    #define ARCH_M68K 68030
#elif defined(__mc68020__) || defined(__MC68020__)
    #define ARCH_M68K 68020
#elif defined(__mc68010__) || defined(__MC68010__)
    #define ARCH_M68K 68010
#elif defined(__mc68000__) || defined(__MC68000__)
    #define ARCH_M68K 68000
#endif
#if defined(ARCH_M68K)
    #define ARCH_M68K_CHECK(version) ((version) <= ARCH_M68K)
#else
    #define ARCH_M68K_CHECK(version) (0)
#endif

/* Xilinx MicroBlaze
   <https://en.wikipedia.org/wiki/MicroBlaze> */
#if defined(__MICROBLAZE__) || defined(__microblaze__)
    #define ARCH_MICROBLAZE 1
#endif

/* MIPS
   <https://en.wikipedia.org/wiki/MIPS_architecture> */
#if defined(_MIPS_ISA_MIPS64R2)
    #define ARCH_MIPS 642
#elif defined(_MIPS_ISA_MIPS64)
    #define ARCH_MIPS 640
#elif defined(_MIPS_ISA_MIPS32R2)
    #define ARCH_MIPS 322
#elif defined(_MIPS_ISA_MIPS32)
    #define ARCH_MIPS 320
#elif defined(_MIPS_ISA_MIPS4)
    #define ARCH_MIPS 4
#elif defined(_MIPS_ISA_MIPS3)
    #define ARCH_MIPS 3
#elif defined(_MIPS_ISA_MIPS2)
    #define ARCH_MIPS 2
#elif defined(_MIPS_ISA_MIPS1)
    #define ARCH_MIPS 1
#elif defined(_MIPS_ISA_MIPS) || defined(__mips) || defined(__MIPS__)
    #define ARCH_MIPS 1
#endif
#if defined(ARCH_MIPS)
    #define ARCH_MIPS_CHECK(version) ((version) <= ARCH_MIPS)
#else
    #define ARCH_MIPS_CHECK(version) (0)
#endif
#if defined(__mips_loongson_mmi)
    #define ARCH_MIPS_LOONGSON_MMI 1
#endif
#if defined(__mips_msa)
    #define ARCH_MIPS_MSA 1
#endif

/* Matsushita MN10300
   <https://en.wikipedia.org/wiki/MN103> */
#if defined(__MN10300__) || defined(__mn10300__)
    #define ARCH_MN10300 1
#endif

/* POWER
   <https://en.wikipedia.org/wiki/IBM_POWER_Instruction_Set_Architecture> */
#if defined(_M_PPC)
    #define ARCH_POWER _M_PPC
#elif defined(_ARCH_PWR9)
    #define ARCH_POWER 900
#elif defined(_ARCH_PWR8)
    #define ARCH_POWER 800
#elif defined(_ARCH_PWR7)
    #define ARCH_POWER 700
#elif defined(_ARCH_PWR6)
    #define ARCH_POWER 600
#elif defined(_ARCH_PWR5)
    #define ARCH_POWER 500
#elif defined(_ARCH_PWR4)
    #define ARCH_POWER 400
#elif defined(_ARCH_440) || defined(__ppc440__)
    #define ARCH_POWER 440
#elif defined(_ARCH_450) || defined(__ppc450__)
    #define ARCH_POWER 450
#elif defined(_ARCH_601) || defined(__ppc601__)
    #define ARCH_POWER 601
#elif defined(_ARCH_603) || defined(__ppc603__)
    #define ARCH_POWER 603
#elif defined(_ARCH_604) || defined(__ppc604__)
    #define ARCH_POWER 604
#elif defined(_ARCH_605) || defined(__ppc605__)
    #define ARCH_POWER 605
#elif defined(_ARCH_620) || defined(__ppc620__)
    #define ARCH_POWER 620
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC) || defined(__ppc)
    #define ARCH_POWER 1
#endif
#if defined(ARCH_POWER)
    #define ARCH_POWER_CHECK(version) ((version) <= ARCH_POWER)
#else
    #define ARCH_POWER_CHECK(version) (0)
#endif
#if defined(__ALTIVEC__)
    #define ARCH_POWER_ALTIVEC ARCH_POWER
    #define ARCH_POWER_ALTIVEC_CHECK(version) ((version) <= ARCH_POWER)
#else
    #define ARCH_POWER_ALTIVEC_CHECK(version) (0)
#endif

/* RISC-V
   <https://en.wikipedia.org/wiki/RISC-V> */
#if defined(__riscv) || defined(__riscv__)
    #if __riscv_xlen == 64
        #define ARCH_RISCV64
    #elif __riscv_xlen == 32
        #define ARCH_RISCV32
    #endif
#endif
/* RISC-V SIMD ISA extensions */
#if defined(__riscv_zve32x)
    #define ARCH_RISCV_ZVE32X 1
#endif
#if defined(__riscv_zve32f)
    #define ARCH_RISCV_ZVE32F 1
#endif
#if defined(__riscv_zve64x)
    #define ARCH_RISCV_ZVE64X 1
#endif
#if defined(__riscv_zve64f)
    #define ARCH_RISCV_ZVE64F 1
#endif
#if defined(__riscv_zve64d)
    #define ARCH_RISCV_ZVE64D 1
#endif
#if defined(__riscv_v)
    #define ARCH_RISCV_V 1
#endif
#if defined(__riscv_zvfh)
    #define ARCH_RISCV_ZVFH 1
#endif
#if defined(__riscv_zvfhmin)
    #define ARCH_RISCV_ZVFHMIN 1
#endif

/* SPARC
   <https://en.wikipedia.org/wiki/SPARC> */
#if defined(__sparc_v9__) || defined(__sparcv9)
    #define ARCH_SPARC 9
#elif defined(__sparc_v8__) || defined(__sparcv8)
    #define ARCH_SPARC 8
#elif defined(__sparc_v7__) || defined(__sparcv7)
    #define ARCH_SPARC 7
#elif defined(__sparc_v6__) || defined(__sparcv6)
    #define ARCH_SPARC 6
#elif defined(__sparc_v5__) || defined(__sparcv5)
    #define ARCH_SPARC 5
#elif defined(__sparc_v4__) || defined(__sparcv4)
    #define ARCH_SPARC 4
#elif defined(__sparc_v3__) || defined(__sparcv3)
    #define ARCH_SPARC 3
#elif defined(__sparc_v2__) || defined(__sparcv2)
    #define ARCH_SPARC 2
#elif defined(__sparc_v1__) || defined(__sparcv1)
    #define ARCH_SPARC 1
#elif defined(__sparc__) || defined(__sparc)
    #define ARCH_SPARC 1
#endif
#if defined(ARCH_SPARC)
    #define ARCH_SPARC_CHECK(version) ((version) <= ARCH_SPARC)
#else
    #define ARCH_SPARC_CHECK(version) (0)
#endif

/* SuperH
   <https://en.wikipedia.org/wiki/SuperH> */
#if defined(__sh5__) || defined(__SH5__)
    #define ARCH_SUPERH 5
#elif defined(__sh4__) || defined(__SH4__)
    #define ARCH_SUPERH 4
#elif defined(__sh3__) || defined(__SH3__)
    #define ARCH_SUPERH 3
#elif defined(__sh2__) || defined(__SH2__)
    #define ARCH_SUPERH 2
#elif defined(__sh1__) || defined(__SH1__)
    #define ARCH_SUPERH 1
#elif defined(__sh__) || defined(__SH__)
    #define ARCH_SUPERH 1
#endif

/* IBM System z
   <https://en.wikipedia.org/wiki/IBM_System_z> */
#if defined(__370__) || defined(__THW_370__) || defined(__s390__) || defined(__s390x__) || defined(__zarch__) || defined(__SYSC_ZARCH__)
    #define ARCH_ZARCH __ARCH__
#endif
#if defined(ARCH_ZARCH)
    #define ARCH_ZARCH_CHECK(version) ((version) <= ARCH_ZARCH)
#else
    #define ARCH_ZARCH_CHECK(version) (0)
#endif

#if defined(ARCH_ZARCH) && defined(__VEC__)
  #define ARCH_ZARCH_ZVECTOR ARCH_ZARCH
#endif

/* TMS320 DSP
   <https://en.wikipedia.org/wiki/Texas_Instruments_TMS320> */
#if defined(_TMS320C6740) || defined(__TMS320C6740__)
    #define ARCH_TMS320 6740
#elif defined(_TMS320C6700_PLUS) || defined(__TMS320C6700_PLUS__)
    #define ARCH_TMS320 6701
#elif defined(_TMS320C6700) || defined(__TMS320C6700__)
    #define ARCH_TMS320 6700
#elif defined(_TMS320C6600) || defined(__TMS320C6600__)
    #define ARCH_TMS320 6600
#elif defined(_TMS320C6400_PLUS) || defined(__TMS320C6400_PLUS__)
    #define ARCH_TMS320 6401
#elif defined(_TMS320C6400) || defined(__TMS320C6400__)
    #define ARCH_TMS320 6400
#elif defined(_TMS320C6200) || defined(__TMS320C6200__)
    #define ARCH_TMS320 6200
#elif defined(_TMS320C55X) || defined(__TMS320C55X__)
    #define ARCH_TMS320 550
#elif defined(_TMS320C54X) || defined(__TMS320C54X__)
    #define ARCH_TMS320 540
#elif defined(_TMS320C28X) || defined(__TMS320C28X__)
    #define ARCH_TMS320 280
#endif
#if defined(ARCH_TMS320)
  #define ARCH_TMS320_CHECK(version) ((version) <= ARCH_TMS320)
#else
  #define ARCH_TMS320_CHECK(version) (0)
#endif

/* WebAssembly */
#if defined(__wasm__)
    #define ARCH_WASM 1
#endif
#if defined(ARCH_WASM) && defined(__wasm_simd128__)
    #define ARCH_WASM_SIMD128
    #include <wasm_simd128.h>
#endif
#if defined(ARCH_WASM) && defined(__wasm_relaxed_simd__)
    #define ARCH_WASM_RELAXED_SIMD
#endif

/* Xtensa
   <https://en.wikipedia.org/wiki/> */
#if defined(__xtensa__) || defined(__XTENSA__)
    #define ARCH_XTENSA 1
#endif
/* Availability of 16-bit floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
    #define ARCH_ARM_NEON_FP16
#endif
/* Availability of 16-bit brain floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)
    #define ARCH_ARM_NEON_BF16
#endif

/* LoongArch
   <https://en.wikipedia.org/wiki/Loongson#LoongArch> */
#if defined(__loongarch32)
    #define ARCH_LOONGARCH 1
#elif defined(__loongarch64)
    #define ARCH_LOONGARCH 2
#endif

/* LSX: LoongArch 128-bits SIMD extension */
#if defined(__loongarch_sx)
    #define ARCH_LOONGARCH_LSX 1
    #include <lsxintrin.h>
#endif

/* LASX: LoongArch 256-bits SIMD extension */
#if defined(__loongarch_asx)
    #define ARCH_LOONGARCH_LASX 2
    #include <lasxintrin.h>
#endif

#if CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            static __inline__ void __attribute__((__always_inline__, __nodebug__))
            _m_prefetch(void *__P) {
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif defined(__MINGW64_VERSION_MAJOR)
    #include <intrin.h>
#endif

#if !defined(ALIGN_MAXIMUM)
    #if defined(CC_MSVC_VERSION)
        #if CC_MSVC_VERSION_CHECK(19, 16, 0)
            /* vs 2017 or newer dont need this */
        #else
            #if defined(ARCH_X86) || defined(ARCH_AMD64)
                #if CC_MSVC_VERSION_CHECK(19, 14, 0)
                    #define ALIGN_PLATFORM_MAXIMUM 64
                #elif CC_MSVC_VERSION_CHECK(16, 0, 0)
                    #define ALIGN_PLATFORM_MAXIMUM 32
                #else
                    #define ALIGN_PLATFORM_MAXIMUM 16
                #endif
            #elif defined(ARCH_ARM) || defined(ARCH_AARCH64)
                #define ALIGN_PLATFORM_MAXIMUM 8
            #endif
        #endif
    #elif defined(CC_IBM_VERSION)
        #define ALIGN_PLATFORM_MAXIMUM 16
    #endif
#endif

#if defined(ALIGN_PLATFORM_MAXIMUM)
    #if ALIGN_PLATFORM_MAXIMUM >= 64
        #define ALIGN_64_ 64
        #define ALIGN_32_ 32
        #define ALIGN_16_ 16
        #define ALIGN_8_  8
    #elif ALIGN_PLATFORM_MAXIMUM >= 32
        #define ALIGN_64_ 32
        #define ALIGN_32_ 32
        #define ALIGN_16_ 16
        #define ALIGN_8_  8
    #elif ALIGN_PLATFORM_MAXIMUM >= 16
        #define ALIGN_64_ 16
        #define ALIGN_32_ 16
        #define ALIGN_16_ 16
        #define ALIGN_8_  8
    #elif ALIGN_PLATFORM_MAXIMUM >= 8
        #define ALIGN_64_ 8
        #define ALIGN_32_ 8
        #define ALIGN_16_ 8
        #define ALIGN_8_  8
    #endif
#else
    #define ALIGN_64_ 64
    #define ALIGN_32_ 32
    #define ALIGN_16_ 16
    #define ALIGN_8_  8
#endif

#if defined(ALIGN_MAXIMUM)
    #define ALIGN_CAP(alignment) (((alignment) < (ALIGN_PLATFORM_MAXIMUM)) ? (alignment) : (ALIGN_PLATFORM_MAXIMUM))
#else
    #define ALIGN_CAP(alignment) (alignment)
#endif

#if __has_attribute(__aligned__) 
    #define ALIGN_TO(Alignment) __attribute__((__aligned__(ALIGN_CAP(Alignment))))
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
    #define ALIGN_TO(Alignment) _Alignas(ALIGN_CAP(Alignment))
#elif (defined(__cplusplus) && (__cplusplus >= 201103L))
    #define ALIGN_TO(Alignment) alignas(ALIGN_CAP(Alignment))
#elif defined(CC_MSVC_VERSION)
    #define ALIGN_TO(Alignment) __declspec(align(Alignment))
#endif
#define ALIGN_TO_64 ALIGN_TO(ALIGN_64_)
#define ALIGN_TO_32 ALIGN_TO(ALIGN_32_)
#define ALIGN_TO_16 ALIGN_TO(ALIGN_16_)
#define ALIGN_TO_8  ALIGN_TO(ALIGN_8_)

#ifdef ARCH_X86_AVX
    #define ALIGN_TO_MAT ALIGN_TO_32
#else
    #define ALIGN_TO_MAT ALIGN_TO_16
#endif

#ifndef HAVE_BUILTIN_ASSUME_ALIGNED
    #if HAS_BUILTIN(__builtin_assume_aligned)
        #define HAVE_BUILTIN_ASSUME_ALIGNED 1
    #elif CC_GNUC_VERSION_CHECK(4,7,0)
        #define HAVE_BUILTIN_ASSUME_ALIGNED 1
    #endif
    #ifndef HAVE_BUILTIN_ASSUME_ALIGNED
        #define HAVE_BUILTIN_ASSUME_ALIGNED 0
    #endif
#endif

#if HAVE_BUILTIN_ASSUME_ALIGNED
    #define ASSUME_ALIGNED(expr, alignment) \
        __builtin_assume_aligned((expr), (alignment))
#else
    #define ASSUME_ALIGNED(expr, alignment) (expr)
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
    #define CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type*)ASSUME_ALIGNED((expr), alignof(type)))
#elif defined(CC_MSVC_VERSION)
    #define CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type*)ASSUME_ALIGNED((expr), __alignof(type)))
#else
    #define CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type*)ASSUME_ALIGNED((expr), __alignof__(type)))
#endif

#define Zero(x)  memset(&(x), 0, sizeof((x)))
#define Zerop(x) memset((x), 0, sizeof(*(x)))
#define Zeroa(x) memset((x), 0, sizeof(x))

#define ArraySize(array) (sizeof(array)/sizeof(array[0]))
#define Clamp(x,a,b)     (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define ClampZO(x)       (Clamp(x, 0, 1))
#define Min(x,y)         (((x) < (y)) ? (x) : (y))
#define Max(x,y)         (((x) > (y)) ? (x) : (y))
#define Swap(type,a,b)   { type temp = a; a = b; b = temp; }

#define ReadFlags(flags,mask)   ((flags) & (mask))   /* check for flags */
#define SetFlags(flags,mask)    ((flags) |= (mask))  /* set the specified flags to 1 */
#define UnsetFlags(flags,mask)  ((flags) &= ~(mask)) /* set the specified flags to 0 */
#define ToggleFlags(flags,mask) ((flags) ^= (mask))  /* toggle the specified flags */

#define ReadBit(flags,bit)     (flags & (1 << bit))   /* check for bit */
#define SetBit(flags,bit)      (flags |= (1 << bit))  /* set the specified bits to 1 */
#define UnsetBit(flags,bit)    (flags &= ~(1 << bit)) /* set the specified bits to 0 */
#define ToggleBit(flags,bit)   (flags ^= (1 << bit))  /* toggle the specified bits */

#ifndef false
    #define false 0
#endif
#ifndef true
    #define true 1
#endif
#ifndef __cplusplus
    typedef _Bool           bool;
#endif

typedef int8_t              i8;
typedef int16_t             i16;
typedef int32_t             i32;
typedef int64_t             i64;

typedef uint8_t             u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

typedef float               f32;
typedef double              f64;

typedef i32                 ivec2[2];
typedef i32                 ivec3[3];
typedef i32                 ivec4[4];

typedef f32                 vec2[2];
typedef f32                 vec3[3];
typedef ALIGN_TO_16 f32     vec4[4];

typedef vec4                quat;

typedef ALIGN_TO_16 vec2    mat2[2];
typedef vec3                mat2x3[2];
typedef vec4                mat2x4[2];

typedef vec3                mat3[3];
typedef vec2                mat3x2[3];
typedef vec4                mat3x4[3];

typedef ALIGN_TO_MAT vec4   mat4[4];
typedef vec2                mat4x2[4];
typedef vec3                mat4x3[4];

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

#ifndef EPSILON
    #define EPSILON 1e-5f
#endif

#ifndef DEG2RAD
    #define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
    #define RAD2DEG (180.0f/PI)
#endif

#ifndef LOG_DISABLE_COLOR
    #ifndef LOG_USE_COLOR
        #define LOG_USE_COLOR 1
    #endif
#else 
    #ifndef LOG_USE_COLOR
        #define LOG_USE_COLOR 0
    #endif
#endif

#ifndef LOG_DISABLE_FILE
    #ifndef LOG_FULL_FILEPATH
        #ifdef LAKE_PLATFORM_WINDOWS
            #define LOG_FILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
        #else
            #define LOG_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
        #endif
    #else
        /* if a full path should be logged */
        #define LOG_FILE __FILE__
    #endif /* LOG_FULL_FILEPATH */
#else
    #define LOG_FILE NULL
#endif

#ifndef LOG_DISABLE_LINE
    #define LOG_LINE __LINE__
#else
    #define LOG_LINE 0
#endif

typedef struct LogMessage {
    va_list     ap;
    const char *fmt;
    const char *file;
    struct tm  *time;
    void       *output;
    i32         line;
    i32         level;
} LogMessage;

typedef enum {
    LOG_VERBOSE = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
} LogLevel;

extern void LogFunctionRaw(char *fmt, ...) PRINTF_FORMAT(1);
extern void LogFunction(i32 level, const char *file, i32 line, const char *fmt, ...) PRINTF_FORMAT(4);

#ifndef LOG_DISABLE_OUTPUT
#define LogRaw(...)     LogFunctionRaw(__VA_ARGS__)
#define LogVerbose(...) LogFunction(LOG_VERBOSE, LOG_FILE, LOG_LINE, __VA_ARGS__)
#define LogDebug(...)   LogFunction(LOG_DEBUG,   LOG_FILE, LOG_LINE, __VA_ARGS__)
#define LogInfo(...)    LogFunction(LOG_INFO,    LOG_FILE, LOG_LINE, __VA_ARGS__)
#define LogWarn(...)    LogFunction(LOG_WARN,    LOG_FILE, LOG_LINE, __VA_ARGS__)
#define LogError(...)   LogFunction(LOG_ERROR,   LOG_FILE, LOG_LINE, __VA_ARGS__)
#define LogFatal(...)   LogFunction(LOG_FATAL,   LOG_FILE, LOG_LINE, __VA_ARGS__)
#else
#define LogRaw(...)
#define LogVerbose(...) 
#define LogDebug(...) 
#define LogInfo(...)  
#define LogWarn(...)  
#define LogError(...) 
#define LogFatal(...) 
#endif

extern i32  LogGetLevel(void);
extern void LogSetLevel(i32 level);
extern bool LogIsQuiet(void);
extern void LogSetQuiet(bool quiet);

#define MS_PER_SECOND       1000
#define US_PER_SECOND       1000000
#define NS_PER_SECOND       1000000000LL
#define NS_PER_MS           1000000
#define NS_PER_US           1000
#define SECONDS_TO_NS(S)    (((u64)(S)) * NS_PER_SECOND)
#define NS_TO_SECONDS(NS)   ((NS) / NS_PER_SECOND)
#define MS_TO_NS(MS)        (((u64)(MS)) * NS_PER_MS)
#define NS_TO_MS(NS)        ((NS) / NS_PER_MS)
#define US_TO_NS(US)        (((u64)(US)) * NS_PER_US)
#define NS_TO_US(NS)        ((NS) / NS_PER_US)

/** A global timer that counts from the moment it's initialized. */
extern void TicksInit(void);
extern void TicksQuit(void);

/** Get the time passed since initializing the 'ticks' timer, in ms or ns. */
extern u64  TicksMS(void);
extern u64  TicksNS(void);

#if HAS_BUILTIN(__builtin_debugtrap)
    #define DebugTrap() __builtin_debugtrap()
#elif HAS_BUILTIN(__debugbreak)
    #define DebugTrap() __debugbreak()
#endif
#if !defined(DebugTrap)
    #if defined(CC_MSVC_VERSION) || defined(CC_INTEL_VERSION)
        #define DebugTrap() __debugbreak()
    #elif defined(CC_ARM_VERSION)
        #define DebugTrap() __breakpoint(42)
    #elif defined(CC_IBM_VERSION)
        #include <builtins.h>
        #define DebugTrap() __trap(42)
    #elif ARCH_X86_CHECK(7) /* _M_IX86 */
        static inline void DebugTrap(void) { __asm int 3h; }
    #elif defined(ARCH_X86) || defined(ARCH_AMD64)
        static inline void DebugTrap(void) { __asm__ __volatile__("int $03"); }
    #elif defined(__thumb__) /* arm32 */
        static inline void DebugTrap(void) { __asm__ __volatile__(".inst 0xde01"); }
    #elif defined(ARCH_AARCH64)
        static inline void DebugTrap(void) { __asm__ __volatile__(".inst 0xd4200000"); }
    #elif defined(ARCH_ARM)
        static inline void DebugTrap(void) { __asm__ __volatile__(".inst 0xe7f001f0"); }
    #elif defined(ARCH_ALPHA) && !defined(__osf__)
        static inline void DebugTrap(void) { __asm__ __volatile__("bpt"); }
    #elif defined(_54_)
        static inline void DebugTrap(void) { __asm__ __volatile__("ESTOP"); }
    #elif defined(_55_)
        static inline void DebugTrap(void) { __asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
    #elif defined(_64P_)
        static inline void DebugTrap(void) { __asm__ __volatile__("SWBP 0"); }
    #elif defined(_6x_)
        static inline void DebugTrap(void) { __asm__ __volatile__("NOP\n .word 0x10000000"); }
    #elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(CC_GNUC_VERSION)
        #define DebugTrap() __builtin_trap()
    #else
        #include <signal.h>
        #if defined(SIGTRAP)
            #define DebugTrap() raise(SIGTRAP)
        #else
            #define DebugTrap() raise(SIGABRT)
        #endif
    #endif
#endif

#if defined(__cplusplus)
    #if (__cplusplus >= 201103L)
        #define StaticAssert(x,desc) static_assert(x, desc)
    #endif
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define StaticAssert(x,desc) static_assert(x, desc)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define StaticAssert(x,desc) _Static_assert(x, desc)
/* GCC 4.6 or later */
#elif CC_GNUC_VERSION_CHECK(4,6,0)
    /* It will work but it may throw a warning:
     * warning: ISO C99 does not support '_Static_assert' [-Wpedantic] */
    #define StaticAssert(x,desc) _Static_assert(x, desc)
#endif
#ifndef StaticAssert
    #define StaticAssert(x,desc)
#endif
#ifndef ASSERT_LEVEL
    #ifdef DEFAULT_ASSERT_LEVEL
        #define ASSERT_LEVEL DEFAULT_ASSERT_LEVEL
    #elif defined(LAKE_DEBUG) || defined(_DEBUG) || defined(DEBUG) || (defined(CC_GNUC_VERSION) && (!defined(__OPTIMIZE__)) || !defined(LAKE_NDEBUG))
        #define ASSERT_LEVEL 2
    #else
        #define ASSERT_LEVEL 1
    #endif
#endif

#define DISABLED_ASSERT(condition)
#define ENABLED_ASSERT(condition)                           \
    do {                                                    \
        if (! (condition)) {                                \
            LogFatal("Assertion '%s' failed.", #condition); \
            DebugTrap();                                    \
        }                                                   \
    } while (false)

/* This assertion is never disabled at any level. */
#define AssertAlways(condition) ENABLED_ASSERT(condition)

#if ASSERT_LEVEL == 0   /* assertions disabled */
    #define Assert(condition)         DISABLED_ASSERT(condition)
    #define AssertRelease(condition)  DISABLED_ASSERT(condition)
    #define AssertParanoid(condition) DISABLED_ASSERT(condition)
#elif ASSERT_LEVEL == 1  /* release settings. */
    #define Assert(condition)         DISABLED_ASSERT(condition)
    #define AssertRelease(condition)  ENABLED_ASSERT(condition)
    #define AssertParanoid(condition) DISABLED_ASSERT(condition)
#elif ASSERT_LEVEL == 2  /* normal settings. */
    #define Assert(condition)         ENABLED_ASSERT(condition)
    #define AssertRelease(condition)  ENABLED_ASSERT(condition)
    #define AssertParanoid(condition) DISABLED_ASSERT(condition)
#elif ASSERT_LEVEL == 3  /* paranoid settings. */
    #define Assert(condition)         ENABLED_ASSERT(condition)
    #define AssertRelease(condition)  ENABLED_ASSERT(condition)
    #define AssertParanoid(condition) ENABLED_ASSERT(condition)
#else
    #error Unknown assertion level. Use: 0-disabled, 1-release, 2-debug, 3-paranoid.
#endif

#define BYTEORDER_LIL_ENDIAN 1234
#define BYTEORDER_BIG_ENDIAN 4321

#ifndef BYTEORDER
    #ifdef LAKE_PLATFORM_LINUX
        #include <endian.h>
        #define BYTEORDER  __BYTE_ORDER
    /* predefs from newer gcc and clang versions: */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define BYTEORDER BYTEORDER_LIL_ENDIAN
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define BYTEORDER BYTEORDER_BIG_ENDIAN
        #else
            #error "Unsupported endianness"
        #endif /**/
    #else
        #if defined(ARCH_HPPA) || \
            defined(ARCH_M68K) || \
            defined(ARCH_MIPS) || \
            defined(ARCH_POWER) || \
            defined(ARCH_SPARC)
            #define BYTEORDER BYTEORDER_BIG_ENDIAN
        #else
            #define BYTEORDER BYTEORDER_LIL_ENDIAN
        #endif
    #endif
#endif

#ifndef FLOATWORDORDER
    /* predefs from newer gcc versions: */
    #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__FLOAT_WORD_ORDER__)
        #if (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define FLOATWORDORDER BYTEORDER_LIL_ENDIAN
        #elif (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define FLOATWORDORDER BYTEORDER_BIG_ENDIAN
        #else
            #error Unsupported endianness
        #endif
    #elif defined(__MAVERICK__)
        /* For Maverick, float words are always little-endian. */
        #define FLOATWORDORDER BYTEORDER_LIL_ENDIAN
    #elif (defined(__arm__) || defined(__thumb__)) && !defined(__VFP_FP__) && !defined(__ARM_EABI__)
        /* For FPA, float words are always big-endian. */
        #define FLOATWORDORDER BYTEORDER_BIG_ENDIAN
    #else
        /* By default, assume that floats words follow the memory system mode. */
        #define FLOATWORDORDER BYTEORDER
    #endif
#endif

/* various modern compilers may have builtin swap */
#if defined(CC_GNUC_VERSION) || defined(CC_CLANG_VERSION)
    #define HAS_BUILTIN_BSWAP16 (HAS_BUILTIN(__builtin_bswap16)) || CC_GNUC_VERSION_CHECK(4,8,0)
    #define HAS_BUILTIN_BSWAP32 (HAS_BUILTIN(__builtin_bswap32)) || CC_GNUC_VERSION_CHECK(4,3,0)
    #define HAS_BUILTIN_BSWAP64 (HAS_BUILTIN(__builtin_bswap64)) || CC_GNUC_VERSION_CHECK(4,3,0)
    /* this one is broken */
    #define HAS_BROKEN_BSWAP CC_GNUC_VERSION_NOT(2,96,0)
#else
    #define AMW_HAS_BUILTIN_BSWAP16 0
    #define AMW_HAS_BUILTIN_BSWAP32 0
    #define AMW_HAS_BUILTIN_BSWAP64 0
    #define AMW_HAS_BROKEN_BSWAP 0
#endif

/* Byte swap 16-bit integer. */
#if HAS_BUILTIN_BSWAP16
    #define Bswap16(x) __builtin_bswap16(x)
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    #pragma intrinsic(_byteswap_ushort)
    #define Bswap16(x) _byteswap_ushort(x)
#elif defined(ARCH_X86) && !HAS_BROKEN_BSWAP
    INLINE u16 Bswap16(u16 x) {
        __asm__("xchgb %b0,%h0": "=q"(x):"0"(x));
        return x;
    }
#elif defined(ARCH_AMD64)
    INLINE u16 Bswap16(u16 x) {
        __asm__("xchgb %b0,%h0": "=Q"(x):"0"(x));
        return x;
    }
#elif defined(ARCH_POWER)
    INLINE u16 Bswap16(u16 x) {
        i32 result;
        __asm__("rlwimi %0,%2,8,16,23": "=&r"(result):"0"(x >> 8), "r"(x));
        return (u16)result;
    }
#elif (defined(ARCH_M68K) && !defined(__mcoldfire__))
    INLINE u16 Bswap16(u16 x) {
        __asm__("rorw #8,%0": "=d"(x): "0"(x):"cc");
        return x;
    }
#elif defined(__WATCOMC__) && defined(__386__)
    extern __inline u16 Bswap16(u16);
    #pragma aux Bswap16 = \
        "xchg al, ah" \
        parm   [ax]   \
        modify [ax];
#else
    INLINE u16 Bswap16(u16 x) {
        return (u16)((x << 8) | (x >> 8));
    }
#endif

/* Byte swap 32-bit integer. */
#if HAS_BUILTIN_BSWAP32
    #define Bswap32(x) __builtin_bswap32(x)
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    #pragma intrinsic(_byteswap_ulong)
    #define Bswap32(x) _byteswap_ulong(x)
#elif defined(ARCH_X86) && !HAS_BROKEN_BSWAP
    INLINE u32 Bswap32(u32 x) {
        __asm__("bswap %0": "=r"(x):"0"(x));
        return x;
    }
#elif defined(ARCH_AMD64)
    INLINE u32 Bswap32(u32 x) {
        __asm__("bswapl %0": "=r"(x):"0"(x));
        return x;
    }
#elif defined(ARCH_POWER)
    INLINE u32 Bswap32(u32 x) {
        u32 result;

        __asm__("rlwimi %0,%2,24,16,23": "=&r"(result): "0" (x>>24),  "r"(x));
        __asm__("rlwimi %0,%2,8,8,15"  : "=&r"(result): "0" (result), "r"(x));
        __asm__("rlwimi %0,%2,24,0,7"  : "=&r"(result): "0" (result), "r"(x));
        return result;
    }
#elif (defined(ARCH_M68K) && !defined(__mcoldfire__))
    INLINE u32 Bswap32(u32 x) {
        __asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0": "=d"(x): "0"(x):"cc");
        return x;
    }
#elif defined(__WATCOMC__) && defined(__386__)
    extern __inline u32 Bswap32(u32);
    #pragma aux Bswap32 = \
        "bswap eax"  \
        parm   [eax] \
        modify [eax];
#else
    INLINE u32 Bswap32(u32 x) {
        return (u32)((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
    }
#endif

/* Byte swap 64-bit integer. */
#if HAS_BUILTIN_BSWAP64
    #define Bswap64(x) __builtin_bswap64(x)
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    #pragma intrinsic(_byteswap_uint64)
    #define Bswap64(x) _byteswap_uint64(x)
#elif defined(ARCH_X86) && !HAS_BROKEN_BSWAP
    INLINE u64 Bswap64(u64 x) {
        union {
            struct {
                u32 a, b;
            } s;
            u64 u;
        } v;
        v.u = x;
        __asm__("bswapl %0 ; bswapl %1 ; xchgl %0,%1"
                : "=r"(v.s.a), "=r"(v.s.b)
                : "0" (v.s.a),  "1"(v.s.b));
        return v.u;
    }
#elif defined(ARCH_AMD64)
    INLINE u64 Bswap64(u64 x) {
        __asm__("bswapq %0": "=r"(x):"0"(x));
        return x;
    }
#elif defined(__WATCOMC__) && defined(__386__)
    extern __inline u64 Bswap64(u64);
    #pragma aux Bswap64 = \
        "bswap eax"     \
        "bswap edx"     \
        "xchg eax,edx"  \
        parm [eax edx]  \
        modify [eax edx];
#else
    INLINE u64 Bswap64(u64 x) {
        u32 hi, lo;

        /* Separate into high and low 32-bit values and swap them */
        lo = (u32)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (u32)(x & 0xFFFFFFFF);
        x = Bswap32(lo);
        x <<= 32;
        x |= Bswap32(hi);
        return (x);
    }
#endif

INLINE f32 Bswapf(f32 x) {
    union {
        f32 f;
        u32 u;
    } swapper;
    swapper.f = x;
    swapper.u = Bswap32(swapper.u);
    return swapper.f;
}

/* remove extra macros */
#undef HAS_BROKEN_BSWAP
#undef HAS_BUILTIN_BSWAP16
#undef HAS_BUILTIN_BSWAP32
#undef HAS_BUILTIN_BSWAP64

#if BYTEORDER == BYTEORDER_LIL_ENDIAN
    #define Bswap16LE(x) (x)
    #define Bswap32LE(x) (x)
    #define Bswap64LE(x) (x)
    #define BswapfLE(x)  (x)
    #define Bswap16BE(x) Bswap16(x)
    #define Bswap32BE(x) Bswap32(x)
    #define Bswap64BE(x) Bswap64(x)
    #define Bswapf(x)    Bswapf(x)
#else
    #define Bswap16LE(x) Bswap16(x)
    #define Bswap32LE(x) Bswap32(x)
    #define Bswap64LE(x) Bswap64(x)
    #define BswapfLE(x)  Bswapf(x)
    #define Bswap16BE(x) (x)
    #define Bswap32BE(x) (x)
    #define Bswap64BE(x) (x)
    #define BswapfBE(x)  (x)
#endif       

#endif /* _LAKE_common_h_ */
