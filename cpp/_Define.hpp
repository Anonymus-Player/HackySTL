#pragma once

#ifndef HSD_NOT_SUPPORTS_CONSTEXPR
	#define HSD_CONSTEXPR constexpr
#else
	#define HSD_CONSTEXPR
#endif

#if defined(_WIN64) || defined(_WIN32) || defined(__WINDOWS__)
	#define HSD_PLATFORM_WINDOWS
#elif defined(__linux) || defined(linux) || defined(__linux__)
	#define HSD_PLATFORM_LINUX
#elif defined(__DragonFly__) || defined(__FreeBSD__) || \
	defined(__NETBSD__) || defined(__OpenBSD__)
	#define HSD_PLATFORM_BSD
#elif defined(__APPLE__) || defined(macintosh) || defined(__MACH__)
	#define HSD_PLATFORM_OSX
#elif defined(__unix) || defined(unix)
	#define HSD_PLATFORM_UNIX
#else
	#define HSD_PLATFORM_UNKNOWN
#endif

#if defined(HSD_PLATFORM_LINUX) || defined(HSD_PLATFORM_BSD) || \
	defined(HSD_PLATFORM_OSX) || defined(HSD_PLATFORM_UNIX)
	#define HSD_PLATFORM_POSIX
#endif

#if defined(__GNUC__)
	#define HSD_COMPILER_GCC
#elif defined(__INTEL_COMPILER)
	#define HSD_COMPILER_INTEL
#elif defined(__clang__)
	#define HSD_COMPILER_CLANG
#elif defined(_MSC_VER)
	#define HSD_COMPILER_MSVC
#else
	#define HSD_COMPILER_UNKNOWN
#endif