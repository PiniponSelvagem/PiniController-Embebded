#include <Arduino.h>
#include <unity.h>
#include <string.h>
#include <utils/convert.hpp>

using namespace pinicore;

// 128 hex chars = 512 bits = 16 words of 32 bit
#define HEX128_LEN      128
#define HEX128_BITS     512
#define HEX128_WORDS    16

static void fillHex(char* buf, uint32_t len, char c) {
    memset(buf, c, len);
    buf[len] = '\0';
}

// ============================================================================
// Tests for stringHex2Array - basic conversion
// ============================================================================

void test_stringHex2Array_single_nibble() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("1", 1, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000001, array[0]);
}

void test_stringHex2Array_all_bits_of_nibble() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("F", 1, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x0000000F, array[0]);
}

void test_stringHex2Array_multiple_nibbles() {
    uint32_t array[1] = {0};
    // Last char is the least significant nibble
    bool result = stringHex2Array("12", 2, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000012, array[0]);
}

void test_stringHex2Array_full_word() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("DEADBEEF", 8, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, array[0]);
}

void test_stringHex2Array_lowercase_equals_uppercase() {
    uint32_t lower[1] = {0};
    uint32_t upper[1] = {0};
    TEST_ASSERT_TRUE(stringHex2Array("abcdef12", 8, 0, 32, lower, 1));
    TEST_ASSERT_TRUE(stringHex2Array("ABCDEF12", 8, 0, 32, upper, 1));
    TEST_ASSERT_EQUAL_HEX32(upper[0], lower[0]);
}

void test_stringHex2Array_zeros_set_no_bits() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("00000000", 8, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000000, array[0]);
}

void test_stringHex2Array_spans_multiple_words() {
    uint32_t array[2] = {0};
    bool result = stringHex2Array("AABBCCDD11223344", 16, 0, 64, array, 2);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x11223344, array[0]);
    TEST_ASSERT_EQUAL_HEX32(0xAABBCCDD, array[1]);
}

void test_stringHex2Array_hexLen_shorter_than_string() {
    uint32_t array[1] = {0};
    // Only the first 'hexLen' chars are converted
    bool result = stringHex2Array("12345678", 2, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000012, array[0]);
}

void test_stringHex2Array_ors_into_existing_bits() {
    uint32_t array[1] = {0x0000000F};
    bool result = stringHex2Array("F0", 2, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x000000FF, array[0]);
}

// ============================================================================
// Tests for stringHex2Array - invalid input
// ============================================================================

void test_stringHex2Array_invalid_char_letter() {
    uint32_t array[1] = {0};
    TEST_ASSERT_FALSE(stringHex2Array("1G", 2, 0, 32, array, 1));
}

void test_stringHex2Array_invalid_char_symbol() {
    uint32_t array[1] = {0};
    TEST_ASSERT_FALSE(stringHex2Array("0x1", 3, 0, 32, array, 1));
}

void test_stringHex2Array_invalid_char_space() {
    uint32_t array[1] = {0};
    TEST_ASSERT_FALSE(stringHex2Array("1 2", 3, 0, 32, array, 1));
}

void test_stringHex2Array_empty_string() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("", 0, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000000, array[0]);
}

// ============================================================================
// Tests for stringHex2Array - sections
// ============================================================================

void test_stringHex2Array_section_word_aligned() {
    uint32_t array[2] = {0};
    // Section 1 of 32 bits starts at bit 32, so it lands on array[1]
    bool result = stringHex2Array("F", 1, 1, 32, array, 2);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000000, array[0]);
    TEST_ASSERT_EQUAL_HEX32(0x0000000F, array[1]);
}

void test_stringHex2Array_section_not_word_aligned() {
    uint32_t array[1] = {0};
    // Section 1 of 8 bits starts at bit 8
    bool result = stringHex2Array("FF", 2, 1, 8, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x0000FF00, array[0]);
}

void test_stringHex2Array_section_offset_into_second_word() {
    uint32_t array[2] = {0};
    // Section 1 of 48 bits starts at bit 48, so it lands on the upper half of array[1]
    bool result = stringHex2Array("FFFF", 4, 1, 48, array, 2);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000000, array[0]);
    TEST_ASSERT_EQUAL_HEX32(0xFFFF0000, array[1]);
}

// ============================================================================
// Range tests, using a 128 char hex string (512 bits / 16 words)
// ============================================================================

void test_range_hex128_fills_whole_section() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, 'F');

    uint32_t array[HEX128_WORDS] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS, array, HEX128_WORDS);
    TEST_ASSERT_TRUE(result);
    for (uint32_t i = 0; i < HEX128_WORDS; ++i) {
        TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, array[i]);
    }
}

void test_range_hex128_lowest_and_highest_bit() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, '0');
    hex[0] = '8';                   // most significant nibble -> bit 511
    hex[HEX128_LEN - 1] = '1';      // least significant nibble -> bit 0

    uint32_t array[HEX128_WORDS] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS, array, HEX128_WORDS);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000001, array[0]);
    TEST_ASSERT_EQUAL_HEX32(0x80000000, array[HEX128_WORDS - 1]);
    for (uint32_t i = 1; i < HEX128_WORDS - 1; ++i) {
        TEST_ASSERT_EQUAL_HEX32(0x00000000, array[i]);
    }
}

void test_range_hex128_last_bit_of_section_fits() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, '0');
    hex[0] = '8'; // bit 511, the last bit of the section

    uint32_t array[HEX128_WORDS] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS, array, HEX128_WORDS);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x80000000, array[HEX128_WORDS - 1]);
}

void test_range_hex128_exceeds_section_by_one_bit() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, '0');
    hex[0] = '8'; // bit 511, one bit past a 511 bit section

    uint32_t array[HEX128_WORDS] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS - 1, array, HEX128_WORDS);
    TEST_ASSERT_FALSE(result);
}

void test_range_hex128_exceeds_section_by_one_nibble() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, 'F');

    uint32_t array[HEX128_WORDS] = {0};
    // Section is 4 bits shorter than the hex string needs
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS - 4, array, HEX128_WORDS);
    TEST_ASSERT_FALSE(result);
}

void test_range_hex128_leading_zeros_do_not_exceed_section() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, '0');
    hex[HEX128_LEN - 1] = 'F'; // only the lowest nibble is used

    uint32_t array[1] = {0};
    // 124 leading zeros are ignored, so a 4 bit section is enough
    bool result = stringHex2Array(hex, HEX128_LEN, 0, 4, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0x0000000F, array[0]);
}

void test_range_hex128_on_second_section() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, 'F');

    uint32_t array[HEX128_WORDS * 2] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 1, HEX128_BITS, array, HEX128_WORDS * 2);
    TEST_ASSERT_TRUE(result);
    for (uint32_t i = 0; i < HEX128_WORDS; ++i) {
        TEST_ASSERT_EQUAL_HEX32(0x00000000, array[i]);
    }
    for (uint32_t i = HEX128_WORDS; i < HEX128_WORDS * 2; ++i) {
        TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, array[i]);
    }
}

void test_range_hex128_invalid_char_at_end_of_range() {
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, 'F');
    hex[0] = 'Z';

    uint32_t array[HEX128_WORDS] = {0};
    bool result = stringHex2Array(hex, HEX128_LEN, 0, HEX128_BITS, array, HEX128_WORDS);
    TEST_ASSERT_FALSE(result);
}
// ============================================================================
// Tests for stringHex2Array - array bounds
// ============================================================================

void test_stringHex2Array_exactly_fills_array() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("FFFFFFFF", 8, 0, 32, array, 1);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, array[0]);
}

void test_stringHex2Array_bit_past_array_end() {
    uint32_t array[1] = {0};
    // Section allows bit 32, but the array only holds bits 0..31
    bool result = stringHex2Array("100000000", 9, 0, 64, array, 1);
    TEST_ASSERT_FALSE(result);
}

void test_stringHex2Array_section_past_array_end() {
    uint32_t array[HEX128_WORDS] = {0};
    char hex[HEX128_LEN + 1];
    fillHex(hex, HEX128_LEN, 'F');

    // Section 1 starts at bit 512, past the end of a 16 word array
    bool result = stringHex2Array(hex, HEX128_LEN, 1, HEX128_BITS, array, HEX128_WORDS);
    TEST_ASSERT_FALSE(result);
}

void test_stringHex2Array_zero_sized_array() {
    uint32_t array[1] = {0};
    bool result = stringHex2Array("1", 1, 0, 32, array, 0);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_HEX32(0x00000000, array[0]);
}
void setup() {
    delay(2000); // give the board/serial monitor time to settle before tests run

    UNITY_BEGIN();

    // basic conversion
    RUN_TEST(test_stringHex2Array_single_nibble);
    RUN_TEST(test_stringHex2Array_all_bits_of_nibble);
    RUN_TEST(test_stringHex2Array_multiple_nibbles);
    RUN_TEST(test_stringHex2Array_full_word);
    RUN_TEST(test_stringHex2Array_lowercase_equals_uppercase);
    RUN_TEST(test_stringHex2Array_zeros_set_no_bits);
    RUN_TEST(test_stringHex2Array_spans_multiple_words);
    RUN_TEST(test_stringHex2Array_hexLen_shorter_than_string);
    RUN_TEST(test_stringHex2Array_ors_into_existing_bits);

    // invalid input
    RUN_TEST(test_stringHex2Array_invalid_char_letter);
    RUN_TEST(test_stringHex2Array_invalid_char_symbol);
    RUN_TEST(test_stringHex2Array_invalid_char_space);
    RUN_TEST(test_stringHex2Array_empty_string);

    // sections
    RUN_TEST(test_stringHex2Array_section_word_aligned);
    RUN_TEST(test_stringHex2Array_section_not_word_aligned);
    RUN_TEST(test_stringHex2Array_section_offset_into_second_word);

    // range, 128 char hex
    RUN_TEST(test_range_hex128_fills_whole_section);
    RUN_TEST(test_range_hex128_lowest_and_highest_bit);
    RUN_TEST(test_range_hex128_last_bit_of_section_fits);
    RUN_TEST(test_range_hex128_exceeds_section_by_one_bit);
    RUN_TEST(test_range_hex128_exceeds_section_by_one_nibble);
    RUN_TEST(test_range_hex128_leading_zeros_do_not_exceed_section);
    RUN_TEST(test_range_hex128_on_second_section);
    RUN_TEST(test_range_hex128_invalid_char_at_end_of_range);

    // array bounds
    RUN_TEST(test_stringHex2Array_exactly_fills_array);
    RUN_TEST(test_stringHex2Array_bit_past_array_end);
    RUN_TEST(test_stringHex2Array_section_past_array_end);
    RUN_TEST(test_stringHex2Array_zero_sized_array);

    UNITY_END();
}

void loop() {}
