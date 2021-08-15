// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#if defined(WIN32) && !defined(STRLIB_STATIC_LIB)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif  // STRLIB_STATIC_LIB

namespace {
static constexpr char8_t HEX_NUMBERS[] = u8"0123456789abcdef";

static const uint32 FLAGS_ZEROPAD              = STR_FORMAT_FLAGS_ZEROPAD;
static const uint32 FLAGS_LEFT                 = STR_FORMAT_FLAGS_LEFT;
static const uint32 FLAGS_SIGN                 = STR_FORMAT_FLAGS_SIGN;
static const uint32 FLAGS_HEX                  = STR_FORMAT_FLAGS_HEX;
static const uint32 FLAGS_OCT                  = STR_FORMAT_FLAGS_OCT;
static const uint32 FLAGS_BIN                  = STR_FORMAT_FLAGS_BIN;
static const uint32 FLAGS_UPPERCASE            = STR_FORMAT_FLAGS_UPPERCASE;
static const uint32 FLAGS_HASH                 = STR_FORMAT_FLAGS_HASH;
static const uint32 FLAGS_EXP                  = STR_FORMAT_FLAGS_EXP;

static const uint32 FLAGS_ADAPT_EXP            = (1u << 9u);
static const uint32 FLAGS_SPACE                = (1u << 10u);
static const uint32 FLAGS_PRECISION            = (1u << 11u);
static const uint32 FLAGS_NO_INSERT_TERMINATOR = (1u << 12u);

static const uint32 FLAGS_CHAR                 = (1u << 13u);
static const uint32 FLAGS_SHORT                = (1u << 14u);
static const uint32 FLAGS_LONG                 = (1u << 15u);
static const uint32 FLAGS_LONG_LONG            = (1u << 16u);
} // namespace ""

// Combining Diacritical Marks
bool IsCDMarks(const uint32 cp) {
    return (0x0300 <= cp && cp <= 0x036f)     // Combining Diacritical Marks
        || (0x1ab0 <= cp && cp <= 0x1aff)     // Combining Diacritical Marks Extended
        || (0x1dc0 <= cp && cp <= 0x1dff)     // Combining Diacritical Marks Supplement
        || (0x20d0 <= cp && cp <= 0x20ff)     // Combining Diacritical Marks Symbols
        || (0xfe20 <= cp && cp <= 0xfe2f);    // Combining Half Marks
}

// Variation Selector
bool IsVariationSelector(const uint32 cp) {
    return (0xe0100 <= cp && cp <= 0xe01ef);
}

/// @fn bool IsDigit(const char8_t c)
STRLIB_API_FUNC bool IsDigit(const char8_t c) {
    return (u8'0' <= c) && (c <= u8'9');
}

/// @fn bool IsHex(const char8_t c)
STRLIB_API_FUNC bool IsHex(const char8_t c) {
    return ((u8'0' <= c) && (c <= u8'9'))
        || ((u8'a' <= c) && (c <= u8'f'))
        || ((u8'A' <= c) && (c <= u8'F'));
}

/// @fn char8_t ToUpper(const char8_t c);
STRLIB_API_FUNC char8_t ToUpper(const char8_t c) {
    if ((u8'a' <= c) && (c <= u8'z')) {
        return u8'A' + (c - u8'a');
    }
    return c;
}

/// @fn char8_t ToLower(const char8_t c);
STRLIB_API_FUNC char8_t ToLower(const char8_t c) {
    if ((u8'A' <= c) && (c <= u8'Z')) {
        return u8'a' + (c - u8'A');
    }
    return c;
}

/// @fn sint32 UTF8StringToSInt32(const char8_t *string, size_t *readLen)
/// @param[in]  string  UTF-8 string
/// @param[out] readLen If string is not nullptr, number of bytes read from string will be written
/// @return Converted signed integer value from string
STRLIB_API_FUNC sint32 UTF8StringToSInt32(const char8_t *string, size_t *readLen) {
    if (string == nullptr) {
        return 0;
    }

    size_t idx = 0;

    bool negative = false;
    if (string[idx] == u8'-') {
        negative = true;
        idx++;
    } else if (string[idx] == u8'+') {
        negative = false;
        idx++;
    }

    size_t len = 0;
    uint32 value = UTF8StringToUInt32(&string[idx], &len);

    if (negative) {
        value = ~value + 1;
    }

    if (readLen != nullptr) {
        readLen[0] = static_cast<size_t>(idx) + len;
    }

    return *reinterpret_cast<sint32 *>(&value);
}

/// @fn uint32 UTF8StringToUInt32(const char8_t *string, size_t *readLen)
/// @param[in]  string  UTF-8 string
/// @param[out] readLen If string is not nullptr, number of bytes read from string will be written
/// @return Converted unsigned integer value from string
STRLIB_API_FUNC uint32 UTF8StringToUInt32(const char8_t *string, size_t *readLen) {
    if (string == nullptr) {
        return 0;
    }

    size_t idx = 0;

    uint32 value = 0;
    while (IsDigit(string[idx])) {
        value = value * 10u + static_cast<sint32>(string[idx] - u8'0');
        idx++;
    }

    if (readLen != nullptr) {
        readLen[0] = idx;
    }

    return value;
}

namespace {
size_t PostIntToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, char8_t *base, size_t baseSize, const bool negative, const size_t width, const uint32 flags) {
    size_t requiredDestBufferSize = max(baseSize, width);
    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        requiredDestBufferSize++;
    }

    if (dstBuffer == nullptr) {
        return requiredDestBufferSize;
    }

    if (requiredDestBufferSize > dstBufferSize) {
        return 0;
    }

    size_t written = 0;

    if ((flags & (FLAGS_ZEROPAD | FLAGS_LEFT)) == 0) {
        // Add space
        for (size_t i = baseSize; i < width; ++i) {
            dstBuffer[written++] = u8' ';
        }

        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }
    } else if ((flags & FLAGS_ZEROPAD) != 0) {
        if (negative || (flags & (FLAGS_SIGN | FLAGS_SPACE)) != 0) {
            // Add sign
            dstBuffer[written++] = base[baseSize - 1];
        } else if ((flags & FLAGS_HASH) != 0) {
            // Add hash
            dstBuffer[written++] = base[baseSize - 1];

            if ((flags & (FLAGS_HEX | FLAGS_BIN)) != 0) {
                dstBuffer[written++] = base[baseSize - 2];
            }
        }

        for (size_t i = baseSize; i < width; ++i) {
            dstBuffer[written++] = u8'0';
        }

        if (negative || (flags & (FLAGS_SIGN | FLAGS_SPACE)) != 0) {
            baseSize--;
        } else if ((flags & FLAGS_HASH) != 0) {
            baseSize--;

            if ((flags & (FLAGS_HEX | FLAGS_BIN)) != 0) {
                baseSize--;
            }
        }

        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }
    } else if ((flags & FLAGS_LEFT) != 0) {
        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }

        // Add space
        for (size_t i = baseSize; i < width; ++i) {
            dstBuffer[written++] = u8' ';
        }
    }

    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        dstBuffer[written++] = u8'\0';
    }

    return written;
}

size_t PostFloatToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, char8_t *base, size_t baseSize, char8_t *frac, size_t fracSize, char8_t *exp, size_t expSize, const bool negative, const size_t width, const size_t fracWidth, const size_t expWidth, const uint32 flags) {
    const size_t bufferSize = baseSize + max(fracSize, fracWidth) + max(expSize, expWidth);

    size_t requiredDestBufferSize = max(bufferSize, width);
    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        requiredDestBufferSize++;
    }

    if (dstBuffer == nullptr) {
        return requiredDestBufferSize;
    }

    if (requiredDestBufferSize > dstBufferSize) {
        return 0;
    }

    size_t written = 0;

    if ((flags & (FLAGS_ZEROPAD | FLAGS_LEFT)) == 0) {
        // Add space
        for (size_t i = bufferSize; i < width; ++i) {
            dstBuffer[written++] = u8' ';
        }

        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }

        // Frac part
        for (size_t i = 1; i <= fracSize; ++i) {
            dstBuffer[written++] = frac[fracSize - i];
        }
        for (size_t i = fracSize; i < fracWidth; ++i) {
            dstBuffer[written++] = u8'0';
        }

        // Exp part
        for (size_t i = 1; i <= expSize; ++i) {
            dstBuffer[written++] = exp[expSize - i];
        }
        for (size_t i = expSize; i < expWidth; ++i) {
            dstBuffer[written++] = u8'0';
        }
    } else if ((flags & FLAGS_ZEROPAD) != 0) {
        if (negative || (flags & (FLAGS_SIGN | FLAGS_SPACE)) != 0) {
            // Add sign
            dstBuffer[written++] = base[baseSize - 1];
        } else if ((flags & FLAGS_HASH) != 0) {
            // Add hash
            dstBuffer[written++] = base[baseSize - 1];

            if ((flags & (FLAGS_HEX | FLAGS_BIN)) != 0) {
                dstBuffer[written++] = base[baseSize - 2];
            }
        }

        for (size_t i = bufferSize; i < width; ++i) {
            dstBuffer[written++] = u8'0';
        }

        if (negative || (flags & (FLAGS_SIGN | FLAGS_SPACE)) != 0) {
            baseSize--;
        } else if ((flags & FLAGS_HASH) != 0) {
            baseSize--;

            if ((flags & (FLAGS_HEX | FLAGS_BIN)) != 0) {
                baseSize--;
            }
        }

        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }

        // Frac part
        for (size_t i = 1; i <= fracSize; ++i) {
            dstBuffer[written++] = frac[fracSize - i];
        }
        for (size_t i = fracSize; i < fracWidth; ++i) {
            dstBuffer[written++] = u8'0';
        }
    } else if ((flags & FLAGS_LEFT) != 0) {
        for (size_t i = 1; i <= baseSize; ++i) {
            dstBuffer[written++] = base[baseSize - i];
        }

        // Frac part
        for (size_t i = 1; i <= fracSize; ++i) {
            dstBuffer[written++] = frac[fracSize - i];
        }
        for (size_t i = fracSize; i < fracWidth; ++i) {
            dstBuffer[written++] = u8'0';
        }

        // Add space
        for (size_t i = bufferSize; i < width; ++i) {
            dstBuffer[written++] = u8' ';
        }
    }

    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        dstBuffer[written++] = u8'\0';
    }

    return written;
}
}   // namespace ""

/// @fn size_t SInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint32 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t SInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint32 value, const size_t width, const uint32 flags) {
    const size_t BUFFER_SIZE = 40;
    const bool negative = (value < 0);
    
    char8_t buffer[BUFFER_SIZE];
    size_t bufferSize = 0;

    if ((flags & FLAGS_HEX) != 0) {
        uint32 tempValue = ((uint32 *)&value)[0];

        do {
            char8_t c = HEX_NUMBERS[tempValue & 0xf];

            if ((flags & FLAGS_UPPERCASE) != 0) {
                c = ToUpper(c);
            }

            buffer[bufferSize++] = c;
            tempValue >>= 4;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);
        
        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'X' : u8'x';
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_OCT) != 0) {
        uint32 tempValue = ((uint32 *)&value)[0];

        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x7);
            tempValue >>= 3;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && bufferSize < BUFFER_SIZE) {
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_BIN) != 0) {
        uint32 tempValue = ((uint32 *)&value)[0];

        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x1);
            tempValue >>= 1;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'B' : u8'b';
            buffer[bufferSize++] = u8'0';
        }
    } else {
        sint32 tempValue = negative ? -value : value;

        do {
            buffer[bufferSize++] = u8'0' + (tempValue % 10);
            tempValue /= 10;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (bufferSize < BUFFER_SIZE) {
            if (negative) {
                buffer[bufferSize++] = u8'-';
            } else if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    } 

    return PostIntToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, negative, width, flags);
}

/// @fn size_t UInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint32 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t UInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint32 value, const size_t width, const uint32 flags) {
    const size_t BUFFER_SIZE = 40;

    char8_t buffer[BUFFER_SIZE];
    size_t bufferSize = 0;

    uint32 tempValue = value;

    if ((flags & FLAGS_HEX) != 0) {
        do {
            char8_t c = HEX_NUMBERS[tempValue & 0xf];

            if ((flags & FLAGS_UPPERCASE) != 0) {
                c = ToUpper(c);
            }

            buffer[bufferSize++] = c;

            tempValue >>= 4;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'X' : u8'x';
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_OCT) != 0) {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x7);

            tempValue >>= 3;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && bufferSize < BUFFER_SIZE) {
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_BIN) != 0) {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x1);

            tempValue >>= 1;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'B' : u8'b';
            buffer[bufferSize++] = u8'0';
        }
    } else {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue % 10);
            tempValue /= 10;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (bufferSize < BUFFER_SIZE) {
            if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    }

    return PostIntToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, false, width, flags);
}

/// @fn size_t SInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint64 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t SInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint64 value, const size_t width, const uint32 flags) {
    const size_t BUFFER_SIZE = 72;
    const bool negative = (value < 0);

    char8_t buffer[BUFFER_SIZE];
    size_t bufferSize = 0;

    if ((flags & FLAGS_HEX) != 0) {
        uint64 tempValue = ((uint64 *)&value)[0];

        do {
            char8_t c = HEX_NUMBERS[tempValue & 0xf];

            if ((flags & FLAGS_UPPERCASE) != 0) {
                c = ToUpper(c);
            }

            buffer[bufferSize++] = c;

            tempValue >>= 4;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'X' : u8'x';
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_OCT) != 0) {
        uint64 tempValue = ((uint64 *)&value)[0];

        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x7);

            tempValue >>= 3;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && bufferSize < BUFFER_SIZE) {
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_BIN) != 0) {
        uint64 tempValue = ((uint64 *)&value)[0];

        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x1);

            tempValue >>= 1;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'B' : u8'b';
            buffer[bufferSize++] = u8'0';
        }
    } else {
        sint64 tempValue = negative ? -value : value;

        do {
            buffer[bufferSize++] = u8'0' + (tempValue % 10);
            tempValue /= 10;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (bufferSize < BUFFER_SIZE) {
            if (negative) {
                buffer[bufferSize++] = u8'-';
            } else if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    }

    return PostIntToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, negative, width, flags);
}

/// @fn size_t UInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint64 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t UInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint64 value, const size_t width, const uint32 flags) {
    const size_t BUFFER_SIZE = 72;

    char8_t buffer[BUFFER_SIZE];
    size_t bufferSize = 0;

    uint64 tempValue = value;

    if ((flags & FLAGS_HEX) != 0) {
        do {
            char8_t c = HEX_NUMBERS[tempValue & 0xf];

            if ((flags & FLAGS_UPPERCASE) != 0) {
                c = ToUpper(c);
            }

            buffer[bufferSize++] = c;

            tempValue >>= 4;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'X' : u8'x';
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_OCT) != 0) {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x7);

            tempValue >>= 3;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && bufferSize < BUFFER_SIZE) {
            buffer[bufferSize++] = u8'0';
        }
    } else if ((flags & FLAGS_BIN) != 0) {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue & 0x1);

            tempValue >>= 1;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (((flags & FLAGS_HASH) != 0) && (bufferSize + 2) <= BUFFER_SIZE) {
            buffer[bufferSize++] = (flags & FLAGS_UPPERCASE) != 0 ? u8'B' : u8'b';
            buffer[bufferSize++] = u8'0';
        }
    } else {
        do {
            buffer[bufferSize++] = u8'0' + (tempValue % 10);
            tempValue /= 10;
        } while (0 != tempValue && bufferSize < BUFFER_SIZE);

        if (bufferSize < BUFFER_SIZE) {
            if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    }

    return PostIntToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, false, width, flags);
}

/// @fn size_t FloatToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const float value, const size_t width, const size_t fracWidth, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits floating point value
/// @param[in]  width         Maximum number of words
/// @param[in]  fracWidth     Maximum number of words for fractional part
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t FloatToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const float value, const size_t width, const size_t fracWidth, const uint32 flags) {
    if ((flags & (FLAGS_HEX | FLAGS_OCT | FLAGS_BIN)) != 0) {
        return SInt32ToUTF8String(dstBuffer, dstBufferSize, *reinterpret_cast<const sint32 *>(&value), width, flags);
    }

    const size_t INT_BUFFER_SIZE  = 32;
    const size_t FRAC_BUFFER_SIZE = 32;
    const bool negative = (value < 0.0f);

    char8_t buffer[INT_BUFFER_SIZE];
    size_t bufferSize = 0;

    char8_t fracBuffer[FRAC_BUFFER_SIZE];
    size_t fracBufferSize = 0;
    
    float absValue = negative ? -value : value;

    if (value != value) {
        if ((bufferSize + 3) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'a';
            buffer[bufferSize++] = u8'n';
        }
    } else if (value < -FLT_MAX) {
        if ((bufferSize + 4) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'f';
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'i';
            buffer[bufferSize++] = u8'-';
        }
    } else if (FLT_MAX < value) {
        if ((bufferSize + 3) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'f';
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'i';
        }

        if (bufferSize < INT_BUFFER_SIZE) {
            if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    } else {
        uint32 intPart = static_cast<uint32>(absValue);
            
        // Integer part
        do {
            buffer[bufferSize++] = u8'0' + (intPart % 10);
            intPart /= 10;
        } while (0 != intPart && bufferSize < INT_BUFFER_SIZE);

        // Sign
        if (bufferSize < INT_BUFFER_SIZE) {
            if (negative) {
                buffer[bufferSize++] = u8'-';
            } else if ((flags & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flags & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }

        uint32 fracPart = static_cast<uint32_t>((absValue - static_cast<uint32>(absValue)) * powf(10.0f, static_cast<float>(min(fracWidth, FRAC_BUFFER_SIZE - 1))) + 0.5f);

        // Fractional part
        if (0 < fracPart) {
            do {
                fracBuffer[fracBufferSize++] = u8'0' + (fracPart % 10);
                fracPart /= 10;
            } while (0 < fracPart && fracBufferSize < FRAC_BUFFER_SIZE);

            if (fracBufferSize < FRAC_BUFFER_SIZE) {
                fracBuffer[fracBufferSize++] = u8'.';
            }
        }
    }

    return PostFloatToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, fracBuffer, fracBufferSize, nullptr, 0, negative, width, fracWidth, 0, flags);
}

/// @fn size_t DoubleToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const double value, const size_t width, const size_t fracWidth, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         64-bits floating point value
/// @param[in]  width         Maximum number of words
/// @param[in]  fracWidth     Maximum number of words for fractional part
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t DoubleToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const double value, const size_t width, const size_t fracWidth, const uint32 flags) {
    if ((flags & (FLAGS_HEX | FLAGS_OCT | FLAGS_BIN)) != 0) {
        return SInt64ToUTF8String(dstBuffer, dstBufferSize, *reinterpret_cast<const sint64 *>(&value), width, flags);
    }

    const size_t INT_BUFFER_SIZE  = 64;
    const size_t FRAC_BUFFER_SIZE = 64;
    const size_t EXP_BUFFER_SIZE  = 5;
    const bool negative = (value < 0.0f);

    char8_t buffer[INT_BUFFER_SIZE];
    size_t bufferSize = 0;

    char8_t fracBuffer[FRAC_BUFFER_SIZE];
    size_t fracBufferSize = 0;

    char8_t expBuffer[EXP_BUFFER_SIZE];
    size_t expBufferSize = 0;

    uint32 flagValue = flags;
    size_t baseWidth = width;
    size_t precision = fracWidth;
    size_t expWidth = 0;

    if (value != value) {
        if ((bufferSize + 3) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'a';
            buffer[bufferSize++] = u8'n';
        }
    } else if (value < -DBL_MAX) {
        if ((bufferSize + 4) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'f';
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'i';
            buffer[bufferSize++] = u8'-';
        }
    } else if (DBL_MAX < value) {
        if ((bufferSize + 3) < INT_BUFFER_SIZE) {
            buffer[bufferSize++] = u8'f';
            buffer[bufferSize++] = u8'n';
            buffer[bufferSize++] = u8'i';
        }

        if (bufferSize < INT_BUFFER_SIZE) {
            if ((flagValue & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flagValue & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }
    } else {
        double absValue = negative ? -value : value;
        sint32 expValue = 0;

        if ((flagValue & FLAGS_EXP) != 0) {
            // 以下の変換を使ってlog10を近似する
            // log10(x) ~= log10(1.5) + (x - 1.5) / (1.5 * ln(10))
            // log10(x) = log(x) / log(10) ~= ln(1.5) / ln(10) + (x - 1.5) / (1.5 * ln(10))
            // log10(d) = (i - Bias) * log(2) / log(10) + log10(d2)
            // この時d2は [1, 2) の範囲となる

            union {
                double f;
                uint64 u;
            } conv;

            conv.f = absValue;

            // 入力値から指数部を取り出す
            sint32 exp2Value = static_cast<sint32>((conv.u >> 52) & 0x7ffu - 1023);
            // d2相当の値を作るため、入力値から指数部を落として範囲を[1, 2)に調整した値を作る
            conv.u = (conv.u & ((1ull << 52u) - 1u)) | (1023ull << 52u);
            // 10進数の指数部を近似
            expValue = static_cast<sint32>(exp2Value * 0.301029995663981 + (conv.f - 1.5) * 0.289529654602167 + 0.176091259055681);

            // now we want to compute 10^expval but we want to be sure it won't overflow
            exp2Value = (int)(expValue * 3.321928094887362 + 0.5);

            const double z = expValue * 2.302585092994046 - exp2Value * 0.6931471805599453;
            const double z2 = z * z;

            conv.u = static_cast<uint64>(exp2Value + 1023) << 52U;

            // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
            conv.f *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));

            // correct for rounding errors
            if (absValue < conv.f) {
                expValue--;
                conv.f /= 10;
            }

            // the exponent format is "%+03d" and largest value is "307", so set aside 4-5 characters
            expWidth = ((-100 < expValue) && (expValue < 100)) ? 4u : 5u;

            // in "%g" mode, "precision" is the number of *significant figures* not decimals
            if ((flagValue & FLAGS_ADAPT_EXP) != 0) {
                if ((1e-4 <= absValue) && (absValue < 1e6)) {
                    if (expValue < static_cast<sint32>(precision)) {
                        precision = static_cast<size_t>(static_cast<sint32>(precision) - expValue - 1);
                    } else {
                        precision = 0;
                    }

                    // make sure _ftoa respects precision
                    flagValue |= FLAGS_PRECISION;

                    // no characters in exponent
                    expWidth = 0u;
                    expValue = 0;
                } else {
                    // we use one sigfig for the whole part
                    if (((flagValue & FLAGS_PRECISION) != 0) && (0 < precision)) {
                        --precision;
                    }
                }
            }

            // will everything fit?
            if (expWidth < baseWidth) {
                // we didn't fall-back so subtract the characters required for the exponent
                baseWidth -= expWidth;
            } else {
                // not enough characters, so go back to default sizing
                baseWidth = 0u;
            }
            if (((flagValue & FLAGS_LEFT) != 0) && (0 < expWidth)) {
                // if we're padding on the right, DON'T pad the floating part
                baseWidth = 0u;
            }

            // rescale the float value
            if (0 < expValue) {
                absValue /= conv.f;
            }
        }

        uint64 intPart = static_cast<uint64>(absValue);

        // Integer part
        do {
            buffer[bufferSize++] = u8'0' + (intPart % 10);
            intPart /= 10;
        } while (0 != intPart && bufferSize < INT_BUFFER_SIZE);

        // Sign
        if (bufferSize < INT_BUFFER_SIZE) {
            if (negative) {
                buffer[bufferSize++] = u8'-';
            } else if ((flagValue & FLAGS_SIGN) != 0) {
                buffer[bufferSize++] = u8'+';
            } else if ((flagValue & FLAGS_SPACE) != 0) {
                buffer[bufferSize++] = u8' ';
            }
        }

        uint64 fracPart = static_cast<uint64_t>((absValue - static_cast<uint64>(absValue)) * pow(10.0, static_cast<double>(min(precision, FRAC_BUFFER_SIZE - 1))) + 0.5);

        // Fractional part
        if (0 < fracPart) {
            do {
                fracBuffer[fracBufferSize++] = u8'0' + (fracPart % 10);
                fracPart /= 10;
            } while (0 != fracPart && fracBufferSize < FRAC_BUFFER_SIZE);

            if (fracBufferSize < FRAC_BUFFER_SIZE) {
                fracBuffer[fracBufferSize++] = u8'.';
            }
        }

        // Exponent part
        if (0 < expWidth) {
            do {
                expBuffer[expBufferSize++] = u8'0' + (expValue % 10);
                expValue /= 10;
            } while (0 != expValue && expBufferSize < EXP_BUFFER_SIZE);

            // Sign
            if (expBufferSize < EXP_BUFFER_SIZE) {
                buffer[bufferSize++] = negative ? u8'-' : u8'+';
            }

            if (expBufferSize < EXP_BUFFER_SIZE) {
                expBuffer[expBufferSize++] = (flagValue & FLAGS_UPPERCASE) != 0 ? u8'E' : u8'e';
            }
        }
    }

    return PostFloatToUTF8String(dstBuffer, dstBufferSize, buffer, bufferSize, fracBuffer, fracBufferSize, expBuffer, expBufferSize, negative, baseWidth, precision, expWidth, flags);
}

/// @fn size_t size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string) {
    return UTF8StringConcat(dstBuffer, dstBufferSize, string, SIZE_MAX, 0, 0);
}

/// @fn size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         number of words
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count) {
    return UTF8StringConcat(dstBuffer, dstBufferSize, string, count, 0, 0);
}

/// @fn size_t size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t width, const uint32 flags) {
    return UTF8StringConcat(dstBuffer, dstBufferSize, string, SIZE_MAX, width, flags);
}

/// @fn size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         number of words
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count, const size_t width, const uint32 flags) {
    if (string == nullptr) {
        return 0;
    }

    size_t len = 0;
    size_t idx = 0;
    while ((string[idx] != u8'\0') && (len < count)) {
        uint32 cp = 0;

        if (string[idx] < 0x80) {
            cp = string[idx];
            ++idx;
        } else if ((string[idx] & 0xe0) == 0xc0) {
            if (string[idx + 1] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((string[idx] & 0x1f) << 6) |
                                        (string[idx + 1] & 0x3f));
            idx += 2;
        } else if ((string[idx] & 0xf0) == 0xe0) {
            if (string[idx + 1] == u8'\0' ||
                string[idx + 2] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((string[idx] & 0x0f) << 12) |
                                       ((string[idx + 1] & 0x3f) << 6) |
                                        (string[idx + 2] & 0x3f));
            idx += 3;
        } else if ((string[idx] & 0xf8) == 0xf0) {
            if (string[idx + 1] == u8'\0' ||
                string[idx + 2] == u8'\0' ||
                string[idx + 3] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((string[idx] & 0x07) << 18) |
                                       ((string[idx + 1] & 0x3f) << 12) |
                                       ((string[idx + 2] & 0x3f) << 6) |
                                        (string[idx + 3] & 0x3f));
            idx += 4;
        } else {
            // Invalid code point
            return 0;
        }

        if (IsCDMarks(cp)) {
            continue;
        }
        if (IsVariationSelector(cp)) {
            continue;
        }

        ++len;
    }

    size_t requiredDstBufferSize = len >= width ? idx : width - len + idx;
    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        requiredDstBufferSize++;
    }

    if (dstBuffer == nullptr) {
        return requiredDstBufferSize;
    }

    if (requiredDstBufferSize > dstBufferSize) {
        return 0;
    }

    size_t offset = 0;
    while (dstBuffer[offset] != u8'\0') {
        ++offset;
    }

    size_t written = 0;

    if ((flags & FLAGS_LEFT) != 0) {
        for (size_t i = 0; i < idx; ++i) {
            dstBuffer[offset + written] = string[i];
            ++written;
        }
        for (size_t i = len; i < width; ++i) {
            dstBuffer[offset + written] = u8' ';
            ++written;
        }
    } else {
        for (size_t i = len; i < width; ++i) {
            dstBuffer[offset + written] = u8' ';
            ++written;
        }
        for (size_t i = 0; i < idx; ++i) {
            dstBuffer[offset + written] = string[i];
            ++written;
        }
    }

    if ((flags & FLAGS_NO_INSERT_TERMINATOR) == 0) {
        dstBuffer[offset + written] = u8'\0';
        ++written;
    }

    return written;
}

/// @brief Get the number of characters in input string
/// @attension Number of characters, not bytes
/// @fn size_t UTF8StringLength(const char8_t *string)
/// @param[in] string Null terminated string
/// @return Number of words
STRLIB_API_FUNC size_t UTF8StringLength(const char8_t *string) {
    if (string == nullptr) {
        return 0;
    }

    size_t len = 0;
    size_t idx = 0;
    while (string[idx] != u8'\0') {
        uint32 cp = 0;

        if (string[idx] < 0x80) {
            cp = string[idx];
            idx++;
        } else if ((string[idx] & 0xe0) == 0xc0) {
            if (string[idx + 1] == u8'\0') {
                return static_cast<size_t>(-1);
            }

            cp = static_cast<char32_t>(((string[idx] & 0x1f) << 6) |
                                        (string[idx + 1] & 0x3f));
            idx += 2;
        } else if ((string[idx] & 0xf0) == 0xe0) {
            if (string[idx + 1] == u8'\0' ||
                string[idx + 2] == u8'\0') {
                return static_cast<size_t>(-1);
            }

            cp = static_cast<char32_t>(((string[idx] & 0x0f) << 12) |
                                       ((string[idx + 1] & 0x3f) << 6) |
                                        (string[idx + 2] & 0x3f));
            idx += 3;
        } else if ((string[idx] & 0xf8) == 0xf0) {
            if (string[idx + 1] == u8'\0' ||
                string[idx + 2] == u8'\0' ||
                string[idx + 3] == u8'\0') {
                return static_cast<size_t>(-1);
            }

            cp = static_cast<char32_t>(((string[idx] & 0x07) << 18) |
                                       ((string[idx + 1] & 0x3f) << 12) |
                                       ((string[idx + 2] & 0x3f) << 6) |
                                        (string[idx + 3] & 0x3f));
            idx += 4;
        } else {
            return static_cast<size_t>(-1);
        }

        if (IsCDMarks(cp)) {
            continue;
        }
        if (IsVariationSelector(cp)) {
            continue;
        }

        len++;
    }

    return len;
}

namespace {
// 適切な開始、終了位置を取得
std::pair<size_t, size_t> UTF8StringGetStartEnd(const char8_t *srcString, const size_t offset, const size_t count) {
    if (srcString == nullptr) {
        return { 0, 0 };
    }

    size_t startIdx = 0;
    size_t endIdx = 0;

    bool foundStartIdx = false;
    bool foundEndIdx = false;

    size_t idx = 0;
    size_t offsetIdx = 0;
    size_t numChars = 0;
    while ((srcString[idx] != u8'\0') && (endIdx <= startIdx)) {
        uint32 cp = 0;

        if (srcString[idx] < 0x80) {
            cp = srcString[idx];
            ++offsetIdx;
        } else if ((srcString[idx] & 0xe0) == 0xc0) {
            if (srcString[idx + 1] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((srcString[idx] & 0x1f) << 6) |
                                        (srcString[idx + 1] & 0x3f));
            offsetIdx += 2;
        } else if ((srcString[idx] & 0xf0) == 0xe0) {
            if (srcString[idx + 1] == u8'\0' ||
                srcString[idx + 2] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((srcString[idx] & 0x0f) << 12) |
                                       ((srcString[idx + 1] & 0x3f) << 6) |
                                        (srcString[idx + 2] & 0x3f));
            offsetIdx += 3;
        } else if ((srcString[idx] & 0xf8) == 0xf0) {
            if (srcString[idx + 1] == u8'\0' ||
                srcString[idx + 2] == u8'\0' ||
                srcString[idx + 3] == u8'\0') {
                --idx;
                break;
            }

            cp = static_cast<char32_t>(((srcString[idx] & 0x07) << 18) |
                                       ((srcString[idx + 1] & 0x3f) << 12) |
                                       ((srcString[idx + 2] & 0x3f) << 6) |
                                       (srcString[idx + 3] & 0x3f));
            offsetIdx += 4;
        } else {
        // Invalid code point
        return { 0, 0 };
        }

        if (IsCDMarks(cp)) {
            continue;
        }
        if (IsVariationSelector(cp)) {
            continue;
        }

        if (offset == numChars) {
            startIdx = idx;
            foundStartIdx = true;
        }

        idx += offsetIdx;
        offsetIdx = 0;

        ++numChars;

        if ((offset + count) == numChars) {
            endIdx = idx;
            foundEndIdx = true;
        }
    }

    if (foundStartIdx && !foundEndIdx) {
        endIdx = idx;
    }

    return { startIdx, endIdx };
}
}
/// @fn size_t UTF8StringCopy(char8_t *dstString, const size_t dstStringSize, const char8_t *srcString, const size_t count, const size_t offset)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         number of characters
/// @param[in]  offset        offset characters
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringCopy(char8_t *dstString, const size_t dstStringSize, const char8_t *srcString, const size_t count, const size_t offset) {
    if (srcString == nullptr || count == 0) {
        return 0;
    }

    auto [startIdx, endIdx] = UTF8StringGetStartEnd(srcString, offset, count);

    if (endIdx <= startIdx) {
        return 0;
    }

    const size_t requiredDstStringSize = endIdx - startIdx + 1;

    if (dstString == nullptr) {
        return requiredDstStringSize;
    }

    if (requiredDstStringSize > dstStringSize) {
        return 0;
    }

    size_t written = 0;

    for (size_t i = startIdx; i < endIdx; ++i) {
        dstString[written++] = srcString[i];
    }

    // Insert Ternimator
    dstString[written++] = u8'\0';

    return written;
}

/// @fn sint32 UTF8StringCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] string1 UTF-8 string
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringCompare(const char8_t *string0, const char8_t *string1) {
    return UTF8StringCompare(string0, 0, SIZE_MAX, string1, 0, SIZE_MAX);
}

/// @fn sint32 UTF8StringCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] offset0 Number of offset characters for string0
/// @param[in] count0  Maximum number of characters to search in string0
/// @param[in] string1 UTF-8 string
/// @param[in] offset1 Number of offset characters for string1
/// @param[in] count1  Maximum number of characters to search in string1
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringCompare(const char8_t *string0, const size_t offset0, const size_t count0, const char8_t *string1, const size_t offset1, const size_t count1) {
    if (string0 == nullptr && string1 == nullptr) {
        return 0;
    } else if (string0 == nullptr) {
        return 1;
    } else if (string1 == nullptr) {
        return -1;
    }

    auto [startIdx0, endIdx0] = UTF8StringGetStartEnd(string0, offset0, count0);
    auto [startIdx1, endIdx1] = UTF8StringGetStartEnd(string1, offset1, count1);

    auto numBytes0 = endIdx0 - startIdx0;
    auto numBytes1 = endIdx1 - startIdx1;

    if (numBytes0 != numBytes1) {
        return static_cast<sint32>(numBytes1) - static_cast<sint32>(numBytes0);
    }

    for (size_t i = 0; i < numBytes0; ++i) {
        if (string0[startIdx0 + i] != string1[startIdx1 + i]) {
            return static_cast<sint32>(string1[startIdx1 + i]) - static_cast<sint32>(string0[startIdx0 + i]);
        }
    }

    return 0;
}

/// @fn sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] string1 UTF-8 string
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const char8_t *string1) {
    return UTF8StringIgnoreCaseCompare(string0, 0, SIZE_MAX, string1, 0, SIZE_MAX);
}

/// @fn sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] offset0 Number of offset characters for string0
/// @param[in] count0  Maximum number of characters to search in string0
/// @param[in] string1 UTF-8 string
/// @param[in] offset1 Number of offset characters for string1
/// @param[in] count1  Maximum number of characters to search in string1
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const size_t offset0, const size_t count0, const char8_t *string1, const size_t offset1, const size_t count1) {
    if (string0 == nullptr && string1 == nullptr) {
        return 0;
    } else if (string0 == nullptr) {
        return 1;
    } else if (string1 == nullptr) {
        return -1;
    }

    auto [startIdx0, endIdx0] = UTF8StringGetStartEnd(string0, offset0, count0);
    auto [startIdx1, endIdx1] = UTF8StringGetStartEnd(string1, offset1, count1);

    auto numBytes0 = endIdx0 - startIdx0;
    auto numBytes1 = endIdx1 - startIdx1;

    if (numBytes0 != numBytes1) {
        return static_cast<sint32>(numBytes1) - static_cast<sint32>(numBytes0);
    }

    for (size_t i = 0; i < numBytes0; ++i) {
        uint32 c0 = ToUpper(string0[startIdx0 + i]);
        uint32 c1 = ToUpper(string1[startIdx1 + i]);

        if (c0 != c1) {
            return static_cast<sint32>(c1) - static_cast<sint32>(c0);
        }
    }

    return 0;
}

/// @fn size_t StringFormatUTF8(char8_t *dest, const size_t destSize, const char8_t *format, ...)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  format        Format-control string
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, ...) {
    va_list va;
    va_start(va, format);
    size_t written = UTF8VStringFormat(dstBuffer, dstBufferSize, format, va);
    va_end(va);

    return written;
}

/// @fn size_t UTF8VStringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, va_list va)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  format        Format-control string
/// @param[in]  va            Variable argument list
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8VStringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, va_list va) {
    if (format == nullptr) {
        return 0;
    }

    // 出力先バッファの使用メモリ量測定
    const bool measureMemory = dstBuffer == nullptr;

    size_t idx = 0;
    size_t written = 0;

    while (format[idx] != u8'\0') {
        // Buffer empty
        if (dstBufferSize <= written) {
            written = dstBufferSize - 1;
            break;
        }

        // Decode
        if ((format[idx] & 0xe0) == 0xc0) {
            if (measureMemory) {
                written += 2;
                idx += 2;
            } else {
                if (((dstBufferSize - written) <= 2)
                    || (format[idx + 1] == u8'\0')) {
                    break;
                }
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
            }
            continue;
        } else if ((format[idx] & 0xf0) == 0xe0) {
            if (measureMemory) {
                written += 3;
                idx += 3;
            } else {
                if (((dstBufferSize - written) <= 3)
                     || (format[idx + 1] == u8'\0')
                     || (format[idx + 2] == u8'\0')) {
                    break;
                }
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
            }
            continue;
        } else if ((format[idx] & 0xf8) == 0xf0) {
            if (measureMemory) {
                written += 4;
                idx += 4;
            } else {
                if (((dstBufferSize - written) <= 4)
                    || (format[idx + 1] == u8'\0')
                    || (format[idx + 2] == u8'\0')
                    || (format[idx + 3] == u8'\0')) {
                    break;
                }
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
                dstBuffer[written++] = format[idx++];
            }
            continue;
        } else if (format[idx] >= 0x80u) {
            // Invalid code
            break;
        }

        if (format[idx] != u8'%') {
            if (measureMemory) {
                ++written;
                ++idx;
            } else {
                dstBuffer[written++] = format[idx++];
            }
            continue;
        }

        ++idx;

        uint32 flags = FLAGS_NO_INSERT_TERMINATOR;

        bool breakFlagLoop = false;
        do {
            if (format[idx] == u8'0') {
                flags |= FLAGS_ZEROPAD;
                idx++;
            } else if (format[idx] == u8'-') {
                flags |= FLAGS_LEFT;
                idx++;
            } else if (format[idx] == u8'+') {
                flags |= FLAGS_SIGN;
                idx++;
            } else if (format[idx] == u8' ') {
                flags |= FLAGS_SPACE;
                idx++;
            } else if (format[idx] == u8'#') {
                flags |= FLAGS_HASH;
                idx++;
            } else {
                breakFlagLoop = true;
            }
        } while (!breakFlagLoop);

        size_t width = 0;
        
        // Width field
        if (IsDigit(format[idx])) {
            size_t readLen = 0;
            width = static_cast<size_t>(UTF8StringToUInt32(&format[idx], &readLen));

            idx += readLen;
        } else if (format[idx] == u8'*') {
            // 引数をフィール幅として使う
            const sint32 w = va_arg(va, sint32);
            if (w < 0) {
                flags |= FLAGS_LEFT;
                width = static_cast<size_t>(-w);
            } else {
                width = static_cast<size_t>(w);
            }

            idx++;
        } else if (format[idx] == u8'\0') {
            continue;
        }

        size_t precision = 0;

        // Precision field
        if (format[idx] == u8'.') {
            flags |= FLAGS_PRECISION;
            idx++;

            if (IsDigit(format[idx])) {
                size_t readLen = 0;
                precision = static_cast<size_t>(UTF8StringToUInt32(&format[idx], &readLen));

                idx += readLen;
            } else if (format[idx] == u8'*') {
                // 引数をフィール幅として使う
                const sint32 p = va_arg(va, sint32);
                if (p < 0) {
                    flags &= ~FLAGS_PRECISION;
                } else {
                    precision = static_cast<size_t>(p);
                }

                idx++;
            } else if (format[idx] == u8'\0') {
                continue;
            }
        } else if (format[idx] == u8'\0') {
            continue;
        }

        // Length field
        switch (format[idx]) {
        case u8'l':
            flags = FLAGS_LONG;
            idx++;
            if (format[idx] == u8'l') {
                flags = FLAGS_LONG_LONG;
                idx++;
            }
            break;

        case u8'h':
            flags = FLAGS_SHORT;
            idx++;
            if (format[idx] == u8'h') {
                flags = FLAGS_CHAR;
                idx++;
            }
            break;

        case u8't':
            flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
            idx++;
            break;

        case u8'j':
            flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
            idx++;
            break;

        case u8'z':
            flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
            idx++;
            break;

        default:
            break;
        }

        // Specifier
        if (false
            || (format[idx] == u8'd')
            || (format[idx] == u8'i')
            || (format[idx] == u8'u')
            || (format[idx] == u8'x')
            || (format[idx] == u8'X')
            || (format[idx] == u8'o')
            || (format[idx] == u8'b')) {
            if ((format[idx] == u8'x') || (format[idx] == u8'X')) {
                flags |= FLAGS_HEX;
                if (format[idx] == u8'X') {
                    flags |= FLAGS_UPPERCASE;
                }
            } else if (format[idx] == u8'o') {
                flags |= FLAGS_OCT;
            } else if (format[idx] == u8'b') {
                flags |= FLAGS_BIN;
            }

            if ((flags & FLAGS_PRECISION) != 0) {
                flags &= ~FLAGS_ZEROPAD;
            }

            if ((format[idx] == u8'd') || (format[idx] == u8'i')) {
                // signed
                if ((flags & FLAGS_LONG_LONG) != 0) {
                    const sint64 value = va_arg(va, sint64);
                    if (measureMemory) {
                        written += SInt64ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += SInt64ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                } else if ((flags & FLAGS_LONG) != 0) {
                    const sint32 value = va_arg(va, sint32);
                    if (measureMemory) {
                        written += SInt32ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += SInt32ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                } else {
                    const sint32 value = (flags & FLAGS_CHAR) != 0 ? static_cast<sint8>(va_arg(va, sint32)) : (flags & FLAGS_SHORT) != 0 ? static_cast<sint16>(va_arg(va, sint32)) : va_arg(va, sint32);
                    if (measureMemory) {
                        written += SInt32ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += SInt32ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                }
            } else {
                // unsigned
                flags &= ~(FLAGS_SIGN | FLAGS_SPACE);

                if ((flags & FLAGS_LONG_LONG) != 0) {
                    const uint64 value = va_arg(va, uint64);
                    if (measureMemory) {
                        written += UInt64ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += UInt64ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                } else if ((flags & FLAGS_LONG) != 0) {
                    const uint32 value = va_arg(va, uint32);
                    if (measureMemory) {
                        written += UInt32ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += UInt32ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                } else {
                    const uint32 value = (flags & FLAGS_CHAR) != 0 ? static_cast<uint8>(va_arg(va, uint32)) : (flags & FLAGS_SHORT) != 0 ? static_cast<uint16>(va_arg(va, uint32)) : va_arg(va, uint32);
                    if (measureMemory) {
                        written += UInt32ToUTF8String(nullptr, 0, value, width, flags);
                    } else {
                        written += UInt32ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                    }
                }
            }

            ++idx;
        } else if (format[idx] == u8'f' || format[idx] == u8'F') {
            if (format[idx] == u8'F') {
                flags |= FLAGS_UPPERCASE;
            }

            double value = va_arg(va, double);
            if (measureMemory) {
                written += DoubleToUTF8String(nullptr, 0, value, width, precision, flags);
            } else {
                written += DoubleToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, precision, flags);
            }

            ++idx;
        } else if (false
                   || (format[idx] == u8'e')
                   || (format[idx] == u8'E')
                   || (format[idx] == u8'g')
                   || (format[idx] == u8'G')) {
            flags |= FLAGS_EXP;
            if (format[idx] == u8'g' || format[idx] == u8'G') {
                flags |= FLAGS_ADAPT_EXP;
            }
            if (format[idx] == u8'E' || format[idx] == u8'G') {
                flags |= FLAGS_UPPERCASE;
            }

            double value = va_arg(va, double);
            if (measureMemory) {
                written += DoubleToUTF8String(nullptr, 0, value, width, precision, flags);
            } else {
                written += DoubleToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, precision, flags);
            }

            ++idx;
        } else if (format[idx] == u8'c') {
            const char8_t *chr = va_arg(va, char8_t *);
            if (measureMemory) {
                written += UTF8StringConcat(nullptr, 0, chr, 1, 1, flags);
            } else {
                written += UTF8StringConcat(&dstBuffer[written], dstBufferSize - written, chr, 1, 1, flags);
            }

            ++idx;
        } else if (format[idx] == u8's') {
            const char8_t *str = va_arg(va, char8_t*);
            if (measureMemory) {
                written += UTF8StringConcat(nullptr, 0, str, precision, width, flags);
            } else {
                written += UTF8StringConcat(&dstBuffer[written], dstBufferSize - written, str, precision, width, flags);
            }

            ++idx;
        } else if (format[idx] == u8'p') {
            flags |= FLAGS_HEX | FLAGS_ZEROPAD | FLAGS_UPPERCASE;

            width = sizeof(void *) * 2u;
            if ((flags & FLAGS_HASH) != 0) {
                width += 2;
            }

            if (sizeof(void *) == sizeof(uint64)) {
                const uint64 value = reinterpret_cast<uint64>(va_arg(va, void*));
                if (measureMemory) {
                    written += UInt64ToUTF8String(nullptr, 0, value, width, flags);
                } else {
                    written += UInt64ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                }
            } else {
                const uint32 value = static_cast<uint32>(reinterpret_cast<uintptr_t>(va_arg(va, void *)));
                if (measureMemory) {
                    written += UInt32ToUTF8String(nullptr, 0, value, width, flags);
                } else {
                    written += UInt32ToUTF8String(&dstBuffer[written], dstBufferSize - written, value, width, flags);
                }
            }

            ++idx;
        } else if (format[idx] == u8'\0') {
            continue;
        } else {
            if (measureMemory) {
                ++written;
                ++idx;
            } else {
                dstBuffer[written++] = format[idx++];
            }
        }
    }

    // Insert terminater
    if (measureMemory) {
        ++written;
    } else {
        dstBuffer[written++] = u8'\0';
    }

    return written;
}
