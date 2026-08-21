#include <Arduino.h>
#include <unity.h>
#include <utils/crypto.hpp>

using namespace pinicore;

void test_checksum_is_deterministic() {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    uint32_t checksum1 = calculateChecksum(data, sizeof(data));
    uint32_t checksum2 = calculateChecksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT32(checksum1, checksum2);
}

void test_checksum_changes_with_phrase() {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    uint32_t withoutPhrase = calculateChecksum(data, sizeof(data));
    uint32_t withPhrase = calculateChecksum(data, sizeof(data), 0x42);
    TEST_ASSERT_NOT_EQUAL(withoutPhrase, withPhrase);
}

void test_phrase_is_deterministic() {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    uint32_t checksum1 = calculateChecksum(data, sizeof(data), 0x42);
    uint32_t checksum2 = calculateChecksum(data, sizeof(data), 0x42);
    TEST_ASSERT_EQUAL_UINT32(checksum1, checksum2);
}

void test_different_phrases_produce_different_checksums() {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    uint32_t checksum_phrase1 = calculateChecksum(data, sizeof(data), 0x42);
    uint32_t checksum_phrase2 = calculateChecksum(data, sizeof(data), 0x43);
    TEST_ASSERT_NOT_EQUAL(checksum_phrase1, checksum_phrase2);
}

void test_phrase_zero_behaves_as_no_phrase() {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    uint32_t checksum_explicit_zero = calculateChecksum(data, sizeof(data), 0);
    uint32_t checksum_default = calculateChecksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT32(checksum_explicit_zero, checksum_default);
}

void test_checksum_changes_with_data() {
    const uint8_t dataA[] = {1, 2, 3};
    const uint8_t dataB[] = {1, 2, 4};
    uint32_t checksumA = calculateChecksum(dataA, sizeof(dataA));
    uint32_t checksumB = calculateChecksum(dataB, sizeof(dataB));
    TEST_ASSERT_NOT_EQUAL(checksumA, checksumB);
}

void setup() {
    delay(2000); // give the board/serial monitor time to settle before tests run

    UNITY_BEGIN();
    RUN_TEST(test_checksum_is_deterministic);
    RUN_TEST(test_checksum_changes_with_phrase);
    RUN_TEST(test_checksum_changes_with_data);
    RUN_TEST(test_phrase_is_deterministic);
    RUN_TEST(test_different_phrases_produce_different_checksums);
    RUN_TEST(test_phrase_zero_behaves_as_no_phrase);
    UNITY_END();
}

void loop() {}
