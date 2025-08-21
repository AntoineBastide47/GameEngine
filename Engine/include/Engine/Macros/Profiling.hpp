//
// Profiling.hpp
// Author: Antoine Bastide
// Date: 23/04/2025
//

#ifndef PROFILING_HPP
#define PROFILING_HPP

#include "Engine/Settings.hpp"
#include "Engine/Macros/Utils.hpp"
#include "Engine/Profiling/Timer.hpp"

using ProfilingLevel = Engine::Settings::Profiling::Level;

#if defined(ENGINE_DEBUG) && defined(ENGINE_PROFILING)
#define ENGINE_PROFILE_SCOPE(name, level) Engine::Profiling::Timer timer##__LINE__(name, level)
#define ENGINE_PROFILE_FUNCTION(level) ENGINE_PROFILE_SCOPE(ENGINE_FUNC_SIG, level)
#else
#define ENGINE_PROFILE_SCOPE(name, level)
#define ENGINE_PROFILE_FUNCTION(level)
#endif

#endif //PROFILING_HPP