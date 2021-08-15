/// @file StringLib.h
/// @author Masayoshi Kamai

#ifndef STRING_LIB_H_
#define STRING_LIB_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


//----------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------------------------------
#if defined(_MSC_VER) || defined(__ICL)
    #define STRLIB_API_IMPORT          __declspec(dllimport)
    #define STRLIB_API_EXPORT          __declspec(dllexport)
    #define STRLIB_API_IMPORT_FUNC     __declspec(dllimport)
    #define STRLIB_API_EXPORT_FUNC     __declspec(dllexport)
    #define STRLIB_API_IMPORT_C_FUNC   extern "C" __declspec(dllimport)
    #define STRLIB_API_EXPORT_C_FUNC   extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
    #define STRLIB_API_IMPORT          __attribute__((visibility("default")))
    #define STRLIB_API_EXPORT          __attribute__((visibility("default")))
    #define STRLIB_API_IMPORT_FUNC     __attribute__((visibility("default")))
    #define STRLIB_API_EXPORT_FUNC     __attribute__((visibility("default")))
    #define STRLIB_API_IMPORT_C_FUNC   __attribute__((visibility("default")))
    #define STRLIB_API_EXPORT_C_FUNC   __attribute__((visibility("default")))
#else
    #define STRLIB_API_IMPORT
    #define STRLIB_API_EXPORT
    #define STRLIB_API_IMPORT_FUNC
    #define STRLIB_API_EXPORT_FUNC
    #define STRLIB_API_IMPORT_C_FUNC
    #define STRLIB_API_EXPORT_C_FUNC
#endif

#if defined(STRLIB_STATIC_LIB)
    #define STRLIB_API
    #define STRLIB_API_FUNC        extern
#elif defined(STRLIB_EXPORTS)
    #define STRLIB_API             STRLIB_API_EXPORT
    #define STRLIB_API_FUNC        STRLIB_API_EXPORT_FUNC
    #define STRLIB_API_C_FUNC      STRLIB_API_EXPORT_C_FUNC
#else
    #define STRLIB_API             STRLIB_API_IMPORT
    #define STRLIB_API_FUNC        STRLIB_API_IMPORT_FUNC
    #define STRLIB_API_C_FUNC      STRLIB_API_IMPORT_C_FUNC
#endif // STRLIB_EXPORTS


//----------------------------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------------------------
#if defined(WIN32) || defined(WIN64) || defined(__WIN32__)
    using sint8  = __int8;
    using uint8  = unsigned __int8;
    using sint16 = __int16;
    using uint16 = unsigned __int16;
    using sint32 = __int32;
    using uint32 = unsigned __int32;
    using sint64 = __int64;
    using uint64 = unsigned __int64;
#else
    using sint8  = char;
    using uint8  = unsigned char;
    using sint16 = short;
    using uint16 = unsigned short;
    using sint32 = long;
    using uint32 = unsigned long;
    using sint64 = long long;
    using uint64 = unsigned long long;
#endif

#if !defined(__cpp_unicode_characters)
    using char16_t = uint16;
    using char32_t = uint32;
#endif
#if !defined(__cpp_char8_t) || ( __cpp_char8_t < 201803)
    using char8_t = char;
#endif


//----------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------

/// @enum TextFormatFlags
enum TextFormatFlags : uint32 {
    STR_FORMAT_FLAGS_ZEROPAD   = (1u << 0u),   ///< Zero padding
    STR_FORMAT_FLAGS_LEFT      = (1u << 1u),   ///< Left alignment
    STR_FORMAT_FLAGS_SIGN      = (1u << 2u),   ///< signed number
    STR_FORMAT_FLAGS_HEX       = (1u << 3u),   ///< Hexadecimal number
    STR_FORMAT_FLAGS_OCT       = (1u << 4u),   ///< Octal number
    STR_FORMAT_FLAGS_BIN       = (1u << 5u),   ///< Binary number
    STR_FORMAT_FLAGS_UPPERCASE = (1u << 6u),   ///< Upper-case notation
    STR_FORMAT_FLAGS_HASH      = (1u << 7u),   ///< Hash code
    STR_FORMAT_FLAGS_EXP       = (1u << 8u),   ///< Exponential expression
};

/// @fn bool IsDigit(const char8_t c)
STRLIB_API_FUNC bool IsDigit(const char8_t c);

/// @fn bool IsHex(const char8_t c)
STRLIB_API_FUNC bool IsHex(const char8_t c);

/// @fn char8_t ToUpper(const char8_t c);
STRLIB_API_FUNC char8_t ToUpper(const char8_t c);

/// @fn char8_t ToLower(const char8_t c);
STRLIB_API_FUNC char8_t ToLower(const char8_t c);

/// @fn sint32 UTF8StringToSInt32(const char8_t *string, size_t *readLen)
/// @param[in]  string  UTF-8 string
/// @param[out] readLen If string is not nullptr, number of bytes read from string will be written
/// @return Converted signed integer value from string
STRLIB_API_FUNC sint32 UTF8StringToSInt32(const char8_t *string, size_t *readLen);

/// @fn uint32 UTF8StringToUInt32(const char8_t *string, size_t *readLen)
/// @param[in]  string  UTF-8 string
/// @param[out] readLen If string is not nullptr, number of bytes read from string will be written
/// @return Converted unsigned integer value from string
STRLIB_API_FUNC uint32 UTF8StringToUInt32(const char8_t *string, size_t *readLen);


/// @fn size_t SInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint32 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of characters
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t SInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint32 value, const size_t width, const uint32 flags);

/// @fn size_t UInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint32 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of characters
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t UInt32ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint32 value, const size_t width, const uint32 flags);

/// @fn size_t SInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint64 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of characters
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t SInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const sint64 value, const size_t width, const uint32 flags);

/// @fn size_t UInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint64 value, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits signed integer value
/// @param[in]  width         Maximum number of characters
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t UInt64ToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const uint64 value, const size_t width, const uint32 flags);

/// @fn size_t FloatToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const float value, const size_t width, const size_t fracWidth, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         32-bits floating point value
/// @param[in]  width         Maximum number of characters
/// @param[in]  fracWidth     Maximum number of characters for fractional part
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t FloatToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const float value, const size_t width, const size_t fracWidth, const uint32 flags);

/// @fn size_t DoubleToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const double value, const size_t width, const size_t fracWidth, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  value         64-bits floating point value
/// @param[in]  width         Maximum number of characters
/// @param[in]  fracWidth     Maximum number of characters for fractional part
/// @param[in]  flags         Format flags
/// @return Number of bytes written
STRLIB_API_FUNC size_t DoubleToUTF8String(char8_t *dstBuffer, const size_t dstBufferSize, const double value, const size_t width, const size_t fracWidth, const uint32 flags);

/// @fn size_t size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string);

/// @fn size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         Maximum number of characters
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count);

/// @brief Get the number of characters in input string
/// @attension Number of characters, not bytes
/// @fn size_t UTF8StringLength(const char8_t *string)
/// @param[in] string Null terminated string
/// @return Number of words
STRLIB_API_FUNC size_t UTF8StringLength(const char8_t *string);

/// @fn size_t UTF8StringCopy(char8_t *dstString, const size_t dstStringSize, const char8_t *srcString, const size_t count, const size_t offset)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         Maximum number of characters
/// @param[in]  offset        Number of offset characters
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringCopy(char8_t *dstString, const size_t dstStringSize, const char8_t *srcString, const size_t count, const size_t offset = 0);

/// @fn sint32 UTF8StringCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] string1 UTF-8 string
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringCompare(const char8_t *string0, const char8_t *string1);

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
STRLIB_API_FUNC sint32 UTF8StringCompare(const char8_t *string0, const size_t offset0, const size_t count0, const char8_t *string1, const size_t offset1, const size_t count1);

/// @fn sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const char8_t *string1)
/// @param[in] string0 UTF-8 string
/// @param[in] string1 UTF-8 string
/// @retval zero     string0 and string1 are equal
/// @retval negative string0 greater than string1
/// @retval positive string0 less than string1
STRLIB_API_FUNC sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const char8_t *string1);

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
STRLIB_API_FUNC sint32 UTF8StringIgnoreCaseCompare(const char8_t *string0, const size_t offset0, const size_t count0, const char8_t *string1, const size_t offset1, const size_t count1);

/// @fn size_t UTF8StringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, ...)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  format        Format-control string
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8StringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, ...);

/// @fn size_t UTF8VStringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, va_list va)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  format        Format-control string
/// @param[in]  va            Variable argument list
/// @return Number of bytes written
STRLIB_API_FUNC size_t UTF8VStringFormat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *format, va_list va);

#endif // STRING_LIB_H_
