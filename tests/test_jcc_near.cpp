#include <gtest/gtest.h>
#include "static_asm.hpp"

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Near conditional jumps use 0F 8x opcodes with 32-bit relative offsets
// Format: 0F 8x rel32 (6 bytes total)

TEST(JccNearInstructions, JoNear) {
    // JO near: 0F 80 rel32
    EXPECT_EQ(jo_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x80, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jo_near(0x00000000), (internal::make_array<std::uint8_t>(0x0F, 0x80, 0x00, 0x00, 0x00, 0x00)));
}

TEST(JccNearInstructions, JnoNear) {
    // JNO near: 0F 81 rel32
    EXPECT_EQ(jno_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x81, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JbNear) {
    // JB/JC/JNAE near: 0F 82 rel32
    EXPECT_EQ(jb_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x82, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jc_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x82, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jnae_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x82, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnbNear) {
    // JAE/JNB/JNC near: 0F 83 rel32
    EXPECT_EQ(jnb_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x83, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jae_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x83, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jnc_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x83, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JzNear) {
    // JE/JZ near: 0F 84 rel32
    EXPECT_EQ(jz_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x84, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(je_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x84, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnzNear) {
    // JNE/JNZ near: 0F 85 rel32
    EXPECT_EQ(jnz_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x85, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jne_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x85, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JbeNear) {
    // JBE/JNA near: 0F 86 rel32
    EXPECT_EQ(jbe_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x86, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jna_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x86, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnbeNear) {
    // JA/JNBE near: 0F 87 rel32
    EXPECT_EQ(jnbe_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x87, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(ja_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x87, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JsNear) {
    // JS near: 0F 88 rel32
    EXPECT_EQ(js_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x88, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnsNear) {
    // JNS near: 0F 89 rel32
    EXPECT_EQ(jns_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x89, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JpNear) {
    // JP/JPE near: 0F 8A rel32
    EXPECT_EQ(jp_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8A, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jpe_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8A, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnpNear) {
    // JNP/JPO near: 0F 8B rel32
    EXPECT_EQ(jnp_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8B, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jpo_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8B, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JlNear) {
    // JL/JNGE near: 0F 8C rel32
    EXPECT_EQ(jl_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8C, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jnge_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8C, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnlNear) {
    // JGE/JNL near: 0F 8D rel32
    EXPECT_EQ(jnl_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8D, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jge_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8D, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JleNear) {
    // JLE/JNG near: 0F 8E rel32
    EXPECT_EQ(jle_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8E, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jng_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8E, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, JnleNear) {
    // JG/JNLE near: 0F 8F rel32
    EXPECT_EQ(jnle_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8F, 0x78, 0x56, 0x34, 0x12)));
    EXPECT_EQ(jg_near(0x12345678), (internal::make_array<std::uint8_t>(0x0F, 0x8F, 0x78, 0x56, 0x34, 0x12)));
}

TEST(JccNearInstructions, NegativeOffsets) {
    // Test with negative offsets (two's complement representation)
    // -1 = 0xFFFFFFFF
    EXPECT_EQ(jz_near(0xFFFFFFFF), (internal::make_array<std::uint8_t>(0x0F, 0x84, 0xFF, 0xFF, 0xFF, 0xFF)));
    // -256 = 0xFFFFFF00
    EXPECT_EQ(jnz_near(0xFFFFFF00), (internal::make_array<std::uint8_t>(0x0F, 0x85, 0x00, 0xFF, 0xFF, 0xFF)));
}

TEST(JccNearInstructions, SmallOffsets) {
    // Small positive offset
    EXPECT_EQ(jz_near(0x00000010), (internal::make_array<std::uint8_t>(0x0F, 0x84, 0x10, 0x00, 0x00, 0x00)));
    // Zero offset
    EXPECT_EQ(jnz_near(0x00000000), (internal::make_array<std::uint8_t>(0x0F, 0x85, 0x00, 0x00, 0x00, 0x00)));
}
