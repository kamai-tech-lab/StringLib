#include "pch.h"

TEST(TestStringLib, IsDigit) {
    EXPECT_TRUE(IsDigit(u8'0'));
    EXPECT_TRUE(IsDigit(u8'1'));
    EXPECT_TRUE(IsDigit(u8'2'));
    EXPECT_TRUE(IsDigit(u8'3'));
    EXPECT_TRUE(IsDigit(u8'4'));
    EXPECT_TRUE(IsDigit(u8'5'));
    EXPECT_TRUE(IsDigit(u8'6'));
    EXPECT_TRUE(IsDigit(u8'7'));
    EXPECT_TRUE(IsDigit(u8'8'));
    EXPECT_TRUE(IsDigit(u8'9'));

    EXPECT_FALSE(IsDigit(u8'a'));
    EXPECT_FALSE(IsDigit(u8'b'));
    EXPECT_FALSE(IsDigit(u8'c'));
    EXPECT_FALSE(IsDigit(u8'd'));
    EXPECT_FALSE(IsDigit(u8'e'));
    EXPECT_FALSE(IsDigit(u8'f'));

    EXPECT_FALSE(IsDigit(u8'A'));
    EXPECT_FALSE(IsDigit(u8'B'));
    EXPECT_FALSE(IsDigit(u8'C'));
    EXPECT_FALSE(IsDigit(u8'D'));
    EXPECT_FALSE(IsDigit(u8'E'));
    EXPECT_FALSE(IsDigit(u8'F'));
}

TEST(TestStringLib, IsHex) {
    EXPECT_TRUE(IsHex(u8'0'));
    EXPECT_TRUE(IsHex(u8'1'));
    EXPECT_TRUE(IsHex(u8'2'));
    EXPECT_TRUE(IsHex(u8'3'));
    EXPECT_TRUE(IsHex(u8'4'));
    EXPECT_TRUE(IsHex(u8'5'));
    EXPECT_TRUE(IsHex(u8'6'));
    EXPECT_TRUE(IsHex(u8'7'));
    EXPECT_TRUE(IsHex(u8'8'));
    EXPECT_TRUE(IsHex(u8'9'));

    EXPECT_TRUE(IsHex(u8'a'));
    EXPECT_TRUE(IsHex(u8'b'));
    EXPECT_TRUE(IsHex(u8'c'));
    EXPECT_TRUE(IsHex(u8'd'));
    EXPECT_TRUE(IsHex(u8'e'));
    EXPECT_TRUE(IsHex(u8'f'));

    EXPECT_TRUE(IsHex(u8'A'));
    EXPECT_TRUE(IsHex(u8'B'));
    EXPECT_TRUE(IsHex(u8'C'));
    EXPECT_TRUE(IsHex(u8'D'));
    EXPECT_TRUE(IsHex(u8'E'));
    EXPECT_TRUE(IsHex(u8'F'));

    EXPECT_FALSE(IsHex(u8'G'));
    EXPECT_FALSE(IsHex(u8'H'));
    EXPECT_FALSE(IsHex(u8'I'));
    EXPECT_FALSE(IsHex(u8'J'));
    EXPECT_FALSE(IsHex(u8'K'));
    EXPECT_FALSE(IsHex(u8'L'));
}

TEST(TestStringLib, ToUpper) {
    EXPECT_EQ(u8'A', ToUpper(u8'a'));
    EXPECT_EQ(u8'B', ToUpper(u8'b'));
    EXPECT_EQ(u8'C', ToUpper(u8'c'));

    EXPECT_EQ(u8'X', ToUpper(u8'x'));
    EXPECT_EQ(u8'Y', ToUpper(u8'y'));
    EXPECT_EQ(u8'Z', ToUpper(u8'z'));

    EXPECT_EQ(u8'0', ToUpper(u8'0'));
    EXPECT_EQ(u8'{', ToUpper(u8'{'));
    EXPECT_EQ(u8'A', ToUpper(u8'A'));
}

TEST(TestStringLib, ToLower) {
    EXPECT_EQ(u8'a', ToLower(u8'A'));
    EXPECT_EQ(u8'b', ToLower(u8'B'));
    EXPECT_EQ(u8'c', ToLower(u8'C'));

    EXPECT_EQ(u8'x', ToLower(u8'X'));
    EXPECT_EQ(u8'y', ToLower(u8'Y'));
    EXPECT_EQ(u8'z', ToLower(u8'Z'));

    EXPECT_EQ(u8'9', ToLower(u8'9'));
    EXPECT_EQ(u8'}', ToLower(u8'}'));
    EXPECT_EQ(u8'z', ToLower(u8'z'));
}

TEST(TestStringLib, UTF8StringToSInt32) {
    // Basic test
    {
        const char8_t testStr[] = u8"12345";
        const sint32 testValue = 12345;

        size_t len = 0;
        sint32 value = UTF8StringToSInt32(testStr, &len);

        EXPECT_EQ(value, testValue);
        EXPECT_EQ(len, sizeof(testStr) / sizeof(testStr[0]) - 1);
    }

    // Sign
    {
        const char8_t testStr[] = u8"+12345";
        const sint32 testValue = 12345;

        size_t len = 0;
        sint32 value = UTF8StringToSInt32(testStr, &len);

        EXPECT_EQ(value, testValue);
        EXPECT_EQ(len, sizeof(testStr) / sizeof(testStr[0]) - 1);
    }

    // Negative
    {
        const char8_t testStr[] = u8"-678910";
        const sint32 testValue = -678910;

        size_t len = 0;
        sint32 value = UTF8StringToSInt32(testStr, &len);

        EXPECT_EQ(value, testValue);
        EXPECT_EQ(len, sizeof(testStr) / sizeof(testStr[0]) - 1);
    }
}

TEST(TestStringLib, UTF8StringToUInt32) {
    {
        const char8_t testStr[] = u8"12345";
        const sint32 testValue = 12345;

        size_t len = 0;
        sint32 value = UTF8StringToUInt32(testStr, &len);

        EXPECT_EQ(value, testValue);
        EXPECT_EQ(len, sizeof(testStr) / sizeof(testStr[0]) - 1);
    }
}

TEST(TestStringLib, SInt32ToUTF8String) {
    // Basic test
    {
        const sint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Buffer empty
    {
        const sint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        char8_t buffer[5] = { u8'\0' };
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, 0);
    }

    // Measure memory
    {
        const sint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        size_t written = SInt32ToUTF8String(nullptr, 0, testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
    }

    // Negative
    {
        const sint32 testValue = -543210;
        const char8_t testStr[] = u8"-543210";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding space right
    {
        const sint32 testValue = -98765;
        const char8_t testStr[] = u8"    -98765";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding space left
    {
        const sint32 testValue = -98765;
        const char8_t testStr[] = u8"-98765    ";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero
    {
        const sint32 testValue = -13579;
        const char8_t testStr[] = u8"-000013579";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Hex
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"cd06";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Hex
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"CD06";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HEX | STR_FORMAT_FLAGS_UPPERCASE);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Negative Hex
    {
        const sint32 testValue = -255;
        const char8_t testStr[] = u8"ffffff01";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero Hex
    {
        const sint32 testValue = 268154;
        const char8_t testStr[] = u8"0004177a";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_ZEROPAD | STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Hex with Hash
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"  0xcd06";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Hex with Hash
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"0XCD06  ";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_HEX | STR_FORMAT_FLAGS_UPPERCASE | STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Oct
    {
        const sint32 testValue = 624891;
        const char8_t testStr[] = u8"2304373";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Negative Oct 
    {
        const sint32 testValue = -27965;
        const char8_t testStr[] = u8"37777711303";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Oct with Hash
    {
        const sint32 testValue = 624891;
        const char8_t testStr[] = u8"02304373";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Bin
    {
        const sint32 testValue = 13;
        const char8_t testStr[] = u8"1101";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_BIN);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Bin with Hash
    {
        const sint32 testValue = 25;
        const char8_t testStr[] = u8"0b11001";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_BIN);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Bin with Hash
    {
        const sint32 testValue = 6;
        const char8_t testStr[] = u8"0B000110";

        char8_t buffer[32];
        size_t written = SInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_BIN | STR_FORMAT_FLAGS_UPPERCASE | STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, UInt32ToUTF8String) {
    // Basic test
    {
        const uint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Buffer empty
    {
        const uint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        char8_t buffer[5] = { u8'\0' };
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, 0);
    }

    // Measure memory
    {
        const uint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";

        size_t written = UInt32ToUTF8String(nullptr, 0, testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
    }

    // Padding space right
    {
        const uint32 testValue = 98765;
        const char8_t testStr[] = u8"     98765";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding space left
    {
        const uint32 testValue = 98765;
        const char8_t testStr[] = u8"98765     ";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero
    {
        const uint32 testValue = 13579;
        const char8_t testStr[] = u8"0000013579";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 10, STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Hex
    {
        const uint32 testValue = 52486;
        const char8_t testStr[] = u8"cd06";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Hex
    {
        const uint32 testValue = 52486;
        const char8_t testStr[] = u8"CD06";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HEX | STR_FORMAT_FLAGS_UPPERCASE);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero Hex
    {
        const uint32 testValue = 268154;
        const char8_t testStr[] = u8"0004177a";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_ZEROPAD | STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Hex with Hash
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"  0xcd06";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_HEX);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Hex with Hash
    {
        const sint32 testValue = 52486;
        const char8_t testStr[] = u8"0XCD06  ";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_HEX | STR_FORMAT_FLAGS_UPPERCASE | STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Oct
    {
        const sint32 testValue = 624891;
        const char8_t testStr[] = u8"2304373";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Negative Oct 
    {
        const sint32 testValue = -27965;
        const char8_t testStr[] = u8"37777711303";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Oct with Hash
    {
        const sint32 testValue = 624891;
        const char8_t testStr[] = u8"02304373";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_OCT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Bin
    {
        const sint32 testValue = 13;
        const char8_t testStr[] = u8"1101";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_BIN);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Bin with Hash
    {
        const sint32 testValue = 25;
        const char8_t testStr[] = u8"0b11001";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 0, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_BIN);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Upper case Bin with Hash
    {
        const sint32 testValue = 6;
        const char8_t testStr[] = u8"0B000110";

        char8_t buffer[32];
        size_t written = UInt32ToUTF8String(buffer, sizeof(buffer), testValue, 8, STR_FORMAT_FLAGS_HASH | STR_FORMAT_FLAGS_BIN | STR_FORMAT_FLAGS_UPPERCASE | STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, SInt64ToUTF8String) {
    // Basic test
    {
        const sint64 testValue = 12345678987654321;
        const char8_t testStr[] = u8"12345678987654321";

        char8_t buffer[32];
        size_t written = SInt64ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, UInt64ToUTF8String) {
    // Basic test
    {
        const uint64 testValue = 12345678987654321;
        const char8_t testStr[] = u8"12345678987654321";

        char8_t buffer[32];
        size_t written = UInt64ToUTF8String(buffer, sizeof(buffer), testValue, 0, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, FloatToUTF8String) {
    // Basic test
    {
        const float testValue = 0.6789f;
        const char8_t testStr[] = u8"0.68";

        char8_t buffer[32];
        size_t written = FloatToUTF8String(buffer, sizeof(buffer), testValue, 0, 2, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Buffer empty
    {
        const float testValue = 12345.6789f;
        const char8_t testStr[] = u8"12345.68";

        char8_t buffer[8] = { u8'\0' };
        size_t written = FloatToUTF8String(buffer, sizeof(buffer), testValue, 0, 2, 0);

        EXPECT_EQ(written, 0);
    }

    // Padding space right
    {
        const float testValue = 1234.5f;
        const char8_t testStr[] = u8"  1234.500";

        char8_t buffer[32];
        size_t written = FloatToUTF8String(buffer, sizeof(buffer), testValue, 10, 3, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding space left
    {
        const float testValue = 1234.5f;
        const char8_t testStr[] = u8"1234.500  ";

        char8_t buffer[32];
        size_t written = FloatToUTF8String(buffer, sizeof(buffer), testValue, 10, 3, STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero
    {
        const float testValue = -1234.5f;
        const char8_t testStr[] = u8"-0001234.5";

        char8_t buffer[32];
        size_t written = FloatToUTF8String(buffer, sizeof(buffer), testValue, 10, 1, STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, DoubleToUTF8String) {
    // Basic test
    {
        const double testValue = 0.6789;
        const char8_t testStr[] = u8"0.68";

        char8_t buffer[32];
        size_t written = DoubleToUTF8String(buffer, sizeof(buffer), testValue, 0, 2, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Buffer empty
    {
        const double testValue = 12345.6789;
        const char8_t testStr[] = u8"12345.68";

        char8_t buffer[8] = { u8'\0' };
        size_t written = DoubleToUTF8String(buffer, sizeof(buffer), testValue, 0, 2, 0);

        EXPECT_EQ(written, 0);
    }

    // Padding space right
    {
        const double testValue = 1234.5;
        const char8_t testStr[] = u8"  1234.500";

        char8_t buffer[32];
        size_t written = DoubleToUTF8String(buffer, sizeof(buffer), testValue, 10, 3, 0);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding space left
    {
        const double testValue = 1234.5;
        const char8_t testStr[] = u8"1234.500  ";

        char8_t buffer[32];
        size_t written = DoubleToUTF8String(buffer, sizeof(buffer), testValue, 10, 3, STR_FORMAT_FLAGS_LEFT);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }

    // Padding Zero
    {
        const double testValue = -1234.5;
        const char8_t testStr[] = u8"-0001234.5";

        char8_t buffer[32];
        size_t written = DoubleToUTF8String(buffer, sizeof(buffer), testValue, 10, 1, STR_FORMAT_FLAGS_ZEROPAD);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}

TEST(TestStringLib, UTF8StringConcat) {
    {
        const char8_t testStr[] = u8"Hello World";

        char8_t buffer0[32] = u8"Hello ";
        char8_t buffer1[] = u8"World";
        size_t written = UTF8StringConcat(buffer0, sizeof(buffer0), buffer1);

        EXPECT_EQ(written, sizeof(buffer1));
        EXPECT_STREQ(buffer0, testStr);
    }
}

TEST(TestStringLib, UTF8StringLength) {
    // Basic test
    {
        const size_t testValue = 17;
        const char8_t testStr[] = u8"Hello UTF-8 World";

        size_t len = UTF8StringLength(testStr);

        EXPECT_EQ(testValue, len);
    }
}

TEST(TestStringLib, UTF8StringCopy) {
    // Basic test
    {
        const char8_t srcString[] = u8"Hello World";
        const char8_t testStr[] = u8"World";

        char8_t dstString[32] = u8"";
        size_t written = UTF8StringCopy(dstString, sizeof(dstString), srcString, 5 /*count*/, 6 /*offset*/);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(dstString, testStr);
    }

    // Invalid count specification
    {
        const char8_t srcString[] = u8"Hello World";
        const char8_t testStr[] = u8"World";

        char8_t dstString[32] = u8"";
        size_t written = UTF8StringCopy(dstString, sizeof(dstString), srcString, 100 /*count*/, 6 /*offset*/);

        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(dstString, testStr);
    }
}

TEST(TestStringLib, UTF8StringCompare) {
    // Basic test
    {
        const char8_t string0[] = u8"Hello World";
        const char8_t string1[] = u8"Good morning Mr. Linus";
        const char8_t string2[] = u8"Good evening Mr. Linus";

        sint32 result = UTF8StringCompare(nullptr, nullptr);
        EXPECT_EQ(result, 0);

        result = UTF8StringCompare(string0, nullptr);
        EXPECT_LT(result, 0);
        
        result = UTF8StringCompare(nullptr, string0);
        EXPECT_GT(result, 0);

        result = UTF8StringCompare(string0, string0);
        EXPECT_EQ(result, 0);

        result = UTF8StringCompare(string1, string0);
        EXPECT_LT(result, 0);

        result = UTF8StringCompare(string0, string1);
        EXPECT_GT(result, 0);

        result = UTF8StringCompare(string1, string2);
        EXPECT_NE(result, 0);
    }
}

TEST(TestStringLib, UTF8StringIgnoreCaseCompare) {
    // Basic test
    {
        const char8_t string0[] = u8"Hello World";
        const char8_t string1[] = u8"HELLO WORLD";
        const char8_t string2[] = u8"Good evening Mr. Linus";

        sint32 result = UTF8StringIgnoreCaseCompare(nullptr, nullptr);
        EXPECT_EQ(result, 0);

        result = UTF8StringIgnoreCaseCompare(string0, nullptr);
        EXPECT_LT(result, 0);

        result = UTF8StringIgnoreCaseCompare(nullptr, string0);
        EXPECT_GT(result, 0);

        result = UTF8StringIgnoreCaseCompare(string0, string0);
        EXPECT_EQ(result, 0);

        result = UTF8StringIgnoreCaseCompare(string0, string1);
        EXPECT_EQ(result, 0);

        result = UTF8StringCompare(string1, string2);
        EXPECT_NE(result, 0);
    }
}

TEST(TestStringLib, UTF8StringFormat) {
    // Basic int test
    {
        const sint32 testValue = 12345;
        const char8_t testStr[] = u8"12345";
    
        char8_t buffer[32];
        size_t written = UTF8StringFormat(buffer, sizeof(buffer), u8"%d", testValue);
    
        EXPECT_EQ(written, sizeof(testStr));
        EXPECT_STREQ(buffer, testStr);
    }
}
