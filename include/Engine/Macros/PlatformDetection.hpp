//
// PlatformDetection.h
// Author: Antoine Bastide
// Date: 08/03/2025
//

#ifndef PLATFORM_DETECTION_H
#define PLATFORM_DETECTION_H

#include "Engine/Macros/Utils.hpp"

#ifdef _WIN32
#ifdef _WIN64
#define ENGINE_WINDOWS_X64
#error "Windows support is not yet available."
#else
#define ENGINE_WINDOWS_X86
#error "Windows x86 support is not yet available."
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS Simulator support is not yet available."
#elif TARGET_OS_IPHONE == 1
#define ENGINE_IOS
#error "IOS support is not yet available."
#elif TARGET_OS_MAC == 1
#define ENGINE_MACOS
#else
#error "Unknown Apple platform!"
#endif
#elif defined(__ANDROID__)
#define ENGINE_ANDROID
#error "Android support is not yet available."
#elif defined(__linux__)
#define ENGINE_LINUX
#error "Linux support is not yet available."
#else
#error "Unknown platform!"
#endif

#endif //PLATFORM_DETECTION_H
