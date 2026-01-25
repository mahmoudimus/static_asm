#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(JccInstructions, ConditionalJumps) {
    EXPECT_EQ(jb(0x12), (internal::make_array<std::uint8_t>(0x72, 0x12)));
    EXPECT_EQ(jbe(0x12), (internal::make_array<std::uint8_t>(0x76, 0x12)));
    EXPECT_EQ(jl(0x12), (internal::make_array<std::uint8_t>(0x7c, 0x12)));
    EXPECT_EQ(jle(0x12), (internal::make_array<std::uint8_t>(0x7e, 0x12)));
    EXPECT_EQ(jnb(0x12), (internal::make_array<std::uint8_t>(0x73, 0x12)));
    EXPECT_EQ(jnbe(0x12), (internal::make_array<std::uint8_t>(0x77, 0x12)));
    EXPECT_EQ(jnl(0x12), (internal::make_array<std::uint8_t>(0x7d, 0x12)));
    EXPECT_EQ(jnle(0x12), (internal::make_array<std::uint8_t>(0x7f, 0x12)));
    EXPECT_EQ(jno(0x12), (internal::make_array<std::uint8_t>(0x71, 0x12)));
    EXPECT_EQ(jnp(0x12), (internal::make_array<std::uint8_t>(0x7b, 0x12)));
    EXPECT_EQ(jns(0x12), (internal::make_array<std::uint8_t>(0x79, 0x12)));
    EXPECT_EQ(jnz(0x12), (internal::make_array<std::uint8_t>(0x75, 0x12)));
    EXPECT_EQ(jo(0x12), (internal::make_array<std::uint8_t>(0x70, 0x12)));
    EXPECT_EQ(jp(0x12), (internal::make_array<std::uint8_t>(0x7a, 0x12)));
    EXPECT_EQ(js(0x12), (internal::make_array<std::uint8_t>(0x78, 0x12)));
    EXPECT_EQ(jz(0x12), (internal::make_array<std::uint8_t>(0x74, 0x12)));
}
