//
// Assert.h
// Author: Antoine Bastide
// Date: 08/03/2025
//

#ifndef ASSERT_H
#define ASSERT_H

#include "Engine/Macros/Utils.hpp"

#if defined(ENGINE_DEBUG) || defined(GAME_DEBUG)
#if defined(ENGINE_WINDOWS)
#define ENGINE_DEBUG_BREAK() __debugbreak()
#elif defined(ENGINE_LINUX)
#include <signal.h>
#define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
#elif defined(ENGINE_MACOS)
#define ENGINE_DEBUG_BREAK() __builtin_trap()
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define ENGINE_ENABLE_ASSERTS
#else
#define ENGINE_DEBUG_BREAK()
#endif

#ifdef ENGINE_ENABLE_ASSERTS
#include <filesystem>
#include <string>
#include "Engine/Log.hpp"
inline std::string GetAssertLocation(const char *file, const int line) {
  return std::filesystem::path(file).filename().string() + ":" + std::to_string(line);
}

#define _e_ASSERT_IMPL(check, msg, ...) { if(!(check)) { Engine::Log::Error(msg); ENGINE_DEBUG_BREAK(); } }
#define _e_ASSERT_WITH_MSG(check, msg) _e_ASSERT_IMPL(check, "Assertion failed: " msg)

#define _e_ASSERT_NO_MSG(check) _e_ASSERT_IMPL(check, ("Assertion " STRINGIFY_MACRO(check) " failed at " + GetAssertLocation(__FILE__, __LINE__)).c_str())

#define _e_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define _e_ASSERT_GET_MACRO(...) EXPAND_MACRO( _e_ASSERT_GET_MACRO_NAME(__VA_ARGS__, _e_ASSERT_WITH_MSG, _e_ASSERT_NO_MSG) )

#define ENGINE_ASSERT(...) EXPAND_MACRO( _e_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define ENGINE_ASSERT(...)
#endif

#endif //ASSERT_H
