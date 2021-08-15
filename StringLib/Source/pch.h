#ifndef STR_LIB_PCH_H
#define STR_LIB_PCH_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(WIN32) || defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN     // Windows ヘッダーから使用されていない部分を除外します。
    #define NOSHLWAPI               // Shlwapi.hを読み込まないように
    #include <windows.h>
    #include <atlbase.h>
#endif // defined(WIN32) || defined(_WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tchar.h>
#include <math.h>
#include <float.h>

#include <utility>

#include "StringLibImpl.h"

#endif // STR_LIB_PCH_H
