#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(BtInstructions, RegisterToRegister) {
    EXPECT_EQ(bt(ecx, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xA3, 0xD9)));
    EXPECT_EQ(bt(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xA3, 0xD9)));
}

TEST(BtInstructions, ImmediateToRegister) {
    EXPECT_EQ(bt(ecx, 12), (internal::make_array<std::uint8_t>(0x0F, 0xBA, 0xE1, 0x0C)));
    EXPECT_EQ(bt(rcx, 12), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBA, 0xE1, 0x0C)));
}

TEST(BtcInstructions, RegisterToRegister) {
    EXPECT_EQ(btc(ecx, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xBB, 0xD9)));
    EXPECT_EQ(btc(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBB, 0xD9)));
}

TEST(BtcInstructions, ImmediateToRegister) {
    EXPECT_EQ(btc(ecx, 12), (internal::make_array<std::uint8_t>(0x0F, 0xBA, 0xF9, 0x0C)));
    EXPECT_EQ(btc(rcx, 12), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBA, 0xF9, 0x0C)));
}

TEST(BtrInstructions, RegisterToRegister) {
    EXPECT_EQ(btr(ecx, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xB3, 0xD9)));
    EXPECT_EQ(btr(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xB3, 0xD9)));
}

TEST(BtrInstructions, ImmediateToRegister) {
    EXPECT_EQ(btr(ecx, 12), (internal::make_array<std::uint8_t>(0x0F, 0xBA, 0xF1, 0x0C)));
    EXPECT_EQ(btr(rcx, 12), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBA, 0xF1, 0x0C)));
}

TEST(BtsInstructions, RegisterToRegister) {
    EXPECT_EQ(bts(ecx, ebx), (internal::make_array<std::uint8_t>(0x0F, 0xAB, 0xD9)));
    EXPECT_EQ(bts(rcx, rbx), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xAB, 0xD9)));
}

TEST(BtsInstructions, ImmediateToRegister) {
    EXPECT_EQ(bts(ecx, 12), (internal::make_array<std::uint8_t>(0x0F, 0xBA, 0xE9, 0x0C)));
    EXPECT_EQ(bts(rcx, 12), (internal::make_array<std::uint8_t>(0x48, 0x0F, 0xBA, 0xE9, 0x0C)));
}
