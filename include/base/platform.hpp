#pragma once

#if defined _MSC_VER || defined __MINGW32__
    #define BCF_PLATFORM_WINDOWS
    #pragma comment(lib, "ws2_32")
#elif defined __APPLE_CC__ || defined __APPLE__
    #define BCF_PLATFORM_DARWIN
#elif defined __FreeBSD__
    #define BCF_PLATFORM_FREEBSD
#else
    #define BCF_PLATFORM_LINUX
#endif

// OS
#if defined(WIN64) || defined(_WIN64)
    #define OS_WIN64
    #define OS_WIN32
#elif defined(WIN32)|| defined(_WIN32)
    #define OS_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
    #define OS_ANDROID
    #define OS_LINUX
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define OS_LINUX
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_MAC) && TARGET_OS_MAC
        #define OS_MAC
    #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        #define OS_IOS
    #endif
    #define OS_DARWIN
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define OS_FREEBSD
    #define OS_BSD
#elif defined(__NetBSD__)
    #define OS_NETBSD
    #define OS_BSD
#elif defined(__OpenBSD__)
    #define OS_OPENBSD
    #define OS_BSD
#elif defined(sun) || defined(__sun) || defined(__sun__)
    #define OS_SOLARIS
#else
    #error "Untested operating system platform!"
#endif

#if defined(OS_WIN32) || defined(OS_WIN64)
    #undef  OS_UNIX
    #define OS_WIN
#else
    #undef  OS_WIN
    #define OS_UNIX
#endif

// ARCH
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #define ARCH_X64
    #define ARCH_X86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define ARCH_X86
    #define ARCH_X86_32
#elif defined(__aarch64__) || defined(__ARM64__) || defined(_M_ARM64)
    #define ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH_ARM
#elif defined(__mips64__)
    #define ARCH_MIPS64
#elif defined(__mips__)
    #define ARCH_MIPS
#else
    #error "Untested hardware architecture!"
#endif

// ENDIAN
#ifndef BIG_ENDIAN
    #define BIG_ENDIAN      4321
#endif
#ifndef LITTLE_ENDIAN
    #define LITTLE_ENDIAN   1234
#endif
#ifndef NET_ENDIAN
    #define NET_ENDIAN      BIG_ENDIAN
#endif

// BYTE_ORDER
#ifndef BYTE_ORDER
    #if defined(ARCH_X86)  || defined(ARCH_X86_64)   || \
        defined(__ARMEL__) || defined(__AARCH64EL__) || \
        defined(__MIPSEL)  || defined(__MIPS64EL)
        #define BYTE_ORDER  LITTLE_ENDIAN
    #elif defined(__ARMEB__) || defined(__AARCH64EB__) || \
        defined(__MIPSEB)  || defined(__MIPS64EB)
        #define BYTE_ORDER  BIG_ENDIAN
    #elif defined(OS_WIN)
        #define BYTE_ORDER  LITTLE_ENDIAN
    #else
        #error "Unknown byte order!"
    #endif
#endif

namespace bcf
{
// UNPACK_BY_LENGTH_FIELD
enum PackEndian {
    USE_HOST_ENDIAN = BYTE_ORDER,
    USE_LITTEL_ENDIAN = LITTLE_ENDIAN,
    USE_BIG_ENDIAN    = BIG_ENDIAN,
    USE_NET_ENDIAN = USE_BIG_ENDIAN,
};
}
