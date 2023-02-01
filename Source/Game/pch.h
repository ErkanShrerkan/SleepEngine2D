// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <CommonUtilities/MathBundle.hpp>
#include <Engine/CommonUtilities.h>
#include <Engine/DebugConsole.h>
#define pout Singleton<SE::Debug::CDebugConsole>()
#define pend SE::Debug::CDebugConsole::EConsoleStream::EndStream
#define pwarn pout
#define perr pout
#include "macro.h"
#include "Time.h"

#endif //PCH_H
