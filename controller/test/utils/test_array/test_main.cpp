#include <Arduino.h>
#include <unity.h>
#include <utils/array.hpp>

using namespace pinicore;

// ============================================================================
// Tests for calculateBitIndex
// ============================================================================

void test_calculateBitIndex_basic_conversion() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    bool result = calculateBitIndex(10, 8, 0, 0, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(0, wordIndex);
    TEST_ASSERT_EQUAL_UINT32(0, bitIndex);
}

void test_calculateBitIndex_single_array_element() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 1 (32 bits), section size: 8, section: 0, bit: 3
    bool result = calculateBitIndex(1, 8, 0, 3, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(0, wordIndex);
    TEST_ASSERT_EQUAL_UINT32(3, bitIndex);
}

void test_calculateBitIndex_spans_multiple_words() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 2 (64 bits), section size: 16, section: 2, bit: 4
    // Global index = 2 * 16 + 4 = 36
    // wordIndex = 36 / 32 = 1, bitIndex = 36 % 32 = 4
    bool result = calculateBitIndex(2, 16, 2, 4, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(1, wordIndex);
    TEST_ASSERT_EQUAL_UINT32(4, bitIndex);
}

void test_calculateBitIndex_bit_at_boundary() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 1 (32 bits), section size: 8, section: 3, bit: 7 (last bit)
    // Global index = 3 * 8 + 7 = 31
    bool result = calculateBitIndex(1, 8, 3, 7, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(0, wordIndex);
    TEST_ASSERT_EQUAL_UINT32(31, bitIndex);
}

void test_calculateBitIndex_invalid_bit_in_section() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 1, section size: 8, section: 0, bit: 8 (out of range for section)
    bool result = calculateBitIndex(1, 8, 0, 8, &wordIndex, &bitIndex);
    TEST_ASSERT_FALSE(result);
}

void test_calculateBitIndex_exceeds_array_bounds() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 1 (32 bits), section size: 8, section: 4, bit: 0
    // Global index = 4 * 8 = 32 (exceeds 32 bits)
    bool result = calculateBitIndex(1, 8, 4, 0, &wordIndex, &bitIndex);
    TEST_ASSERT_FALSE(result);
}

void test_calculateBitIndex_large_section_size() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Array size: 2, section size: 32, section: 1, bit: 16
    // Global index = 1 * 32 + 16 = 48
    // wordIndex = 48 / 32 = 1, bitIndex = 48 % 32 = 16
    bool result = calculateBitIndex(2, 32, 1, 16, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(1, wordIndex);
    TEST_ASSERT_EQUAL_UINT32(16, bitIndex);
}

void test_calculateBitIndex_zero_section_size() {
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    // Zero section size should cause bit >= sectionSize check to fail
    bool result = calculateBitIndex(1, 0, 0, 0, &wordIndex, &bitIndex);
    TEST_ASSERT_FALSE(result);
}

// ============================================================================
// Tests for calculateSectionBit
// ============================================================================

void test_calculateSectionBit_basic_conversion() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 10, section size: 8, wordIndex: 0, bitIndex: 0
    bool result = calculateSectionBit(10, 8, 0, 0, &section, &bit);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(0, section);
    TEST_ASSERT_EQUAL_UINT32(0, bit);
}

void test_calculateSectionBit_single_array_element() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 1, section size: 8, wordIndex: 0, bitIndex: 5
    bool result = calculateSectionBit(1, 8, 0, 5, &section, &bit);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(0, section);
    TEST_ASSERT_EQUAL_UINT32(5, bit);
}

void test_calculateSectionBit_spans_multiple_words() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 2, section size: 16, wordIndex: 1, bitIndex: 4
    // Global index = 1 * 32 + 4 = 36
    // section = 36 / 16 = 2, bit = 36 % 16 = 4
    bool result = calculateSectionBit(2, 16, 1, 4, &section, &bit);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(2, section);
    TEST_ASSERT_EQUAL_UINT32(4, bit);
}

void test_calculateSectionBit_last_bit() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 1, section size: 8, wordIndex: 0, bitIndex: 31 (last bit)
    // Global index = 31
    // section = 31 / 8 = 3, bit = 31 % 8 = 7
    bool result = calculateSectionBit(1, 8, 0, 31, &section, &bit);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(3, section);
    TEST_ASSERT_EQUAL_UINT32(7, bit);
}

void test_calculateSectionBit_invalid_wordIndex() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 1, wordIndex: 1 (out of bounds)
    bool result = calculateSectionBit(1, 8, 1, 0, &section, &bit);
    TEST_ASSERT_FALSE(result);
}

void test_calculateSectionBit_invalid_bitIndex() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 1, bitIndex: 32 (out of bounds for 32-bit word)
    bool result = calculateSectionBit(1, 8, 0, 32, &section, &bit);
    TEST_ASSERT_FALSE(result);
}

void test_calculateSectionBit_large_section_size() {
    uint32_t section = 0;
    uint32_t bit = 0;
    // Array size: 3, section size: 32, wordIndex: 1, bitIndex: 16
    // Global index = 1 * 32 + 16 = 48
    // section = 48 / 32 = 1, bit = 48 % 32 = 16
    bool result = calculateSectionBit(3, 32, 1, 16, &section, &bit);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(1, section);
    TEST_ASSERT_EQUAL_UINT32(16, bit);
}

// ============================================================================
// Round-trip conversion tests (calculateBitIndex -> calculateSectionBit)
// ============================================================================

void test_roundtrip_conversion_simple() {
    // Forward: section 0, bit 3 -> wordIndex, bitIndex
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    bool result1 = calculateBitIndex(1, 8, 0, 3, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result1);
    
    // Backward: wordIndex, bitIndex -> section, bit
    uint32_t section = 0;
    uint32_t bit = 0;
    bool result2 = calculateSectionBit(1, 8, wordIndex, bitIndex, &section, &bit);
    TEST_ASSERT_TRUE(result2);
    TEST_ASSERT_EQUAL_UINT32(0, section);
    TEST_ASSERT_EQUAL_UINT32(3, bit);
}

void test_roundtrip_conversion_multi_word() {
    // Forward: section 2, bit 4 -> wordIndex, bitIndex
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    bool result1 = calculateBitIndex(2, 16, 2, 4, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result1);
    
    // Backward: wordIndex, bitIndex -> section, bit
    uint32_t section = 0;
    uint32_t bit = 0;
    bool result2 = calculateSectionBit(2, 16, wordIndex, bitIndex, &section, &bit);
    TEST_ASSERT_TRUE(result2);
    TEST_ASSERT_EQUAL_UINT32(2, section);
    TEST_ASSERT_EQUAL_UINT32(4, bit);
}

void test_roundtrip_conversion_boundary() {
    // Forward: section 3, bit 7 (boundary) -> wordIndex, bitIndex
    uint32_t wordIndex = 0;
    uint32_t bitIndex = 0;
    bool result1 = calculateBitIndex(1, 8, 3, 7, &wordIndex, &bitIndex);
    TEST_ASSERT_TRUE(result1);
    
    // Backward: wordIndex, bitIndex -> section, bit
    uint32_t section = 0;
    uint32_t bit = 0;
    bool result2 = calculateSectionBit(1, 8, wordIndex, bitIndex, &section, &bit);
    TEST_ASSERT_TRUE(result2);
    TEST_ASSERT_EQUAL_UINT32(3, section);
    TEST_ASSERT_EQUAL_UINT32(7, bit);
}

void setup() {
    delay(2000); // give the board/serial monitor time to settle before tests run

    UNITY_BEGIN();
    
    // calculateBitIndex tests
    RUN_TEST(test_calculateBitIndex_basic_conversion);
    RUN_TEST(test_calculateBitIndex_single_array_element);
    RUN_TEST(test_calculateBitIndex_spans_multiple_words);
    RUN_TEST(test_calculateBitIndex_bit_at_boundary);
    RUN_TEST(test_calculateBitIndex_invalid_bit_in_section);
    RUN_TEST(test_calculateBitIndex_exceeds_array_bounds);
    RUN_TEST(test_calculateBitIndex_large_section_size);
    RUN_TEST(test_calculateBitIndex_zero_section_size);
    
    // calculateSectionBit tests
    RUN_TEST(test_calculateSectionBit_basic_conversion);
    RUN_TEST(test_calculateSectionBit_single_array_element);
    RUN_TEST(test_calculateSectionBit_spans_multiple_words);
    RUN_TEST(test_calculateSectionBit_last_bit);
    RUN_TEST(test_calculateSectionBit_invalid_wordIndex);
    RUN_TEST(test_calculateSectionBit_invalid_bitIndex);
    RUN_TEST(test_calculateSectionBit_large_section_size);
    
    // Round-trip conversion tests
    RUN_TEST(test_roundtrip_conversion_simple);
    RUN_TEST(test_roundtrip_conversion_multi_word);
    RUN_TEST(test_roundtrip_conversion_boundary);
    
    UNITY_END();
}

void loop() {}
