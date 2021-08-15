/// @file StringLibImpl.h
/// @author Masayoshi Kamai

#ifndef STRING_LIB_IMPL_H_
#define STRING_LIB_IMPL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "StringLib.h"

/// @brief Check to if the input is a Combining Diacritical Marks
/// 
/// @fn bool IsCDMarks(const uint32 cp)
/// @param cp Code point for Unicode
/// @return Return true if the input is a Combining Diacritical Marks, false otherwise
bool IsCDMarks(const uint32 cp);

/// @brief Check to if the input is a Variation Selector
/// 
/// @fn bool IsVariationSelector(const uint32 cp)
/// @param cp Code point for Unicode
/// @return Return true if the input is a Variation Selector, false otherwise
bool IsVariationSelector(const uint32 cp);


/// @fn size_t size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t width, const uint32 flags);

/// @fn size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count, const size_t width, const uint32 flags)
/// @param[out] destBuffer    Storage location for Output string
/// @param[in]  dstBufferSize Maximum number of bytes to store
/// @param[in]  string        UTF-8 string
/// @param[in]  count         number of words
/// @param[in]  width         Maximum number of words
/// @param[in]  flags         Format flags
/// @return Number of bytes written
size_t UTF8StringConcat(char8_t *dstBuffer, const size_t dstBufferSize, const char8_t *string, const size_t count, const size_t width, const uint32 flags);


#endif // STRING_LIB_IMPL_H_
