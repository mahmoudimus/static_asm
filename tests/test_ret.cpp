#include <gtest/gtest.h>
#include "static_asm.hpp"
// Using internal::make_array from static_asm

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

TEST(RetInstructions, Near) {
    EXPECT_EQ(ret(), (internal::make_array<std::uint8_t>(0xC3)));
    EXPECT_EQ(ret(2), (internal::make_array<std::uint8_t>(0xC2, 0x02, 0x00)));
}

TEST(RetfInstructions, Far) {
    EXPECT_EQ(retf(), (internal::make_array<std::uint8_t>(0xCB)));
    EXPECT_EQ(retf(2), (internal::make_array<std::uint8_t>(0xCA, 0x02, 0x00)));
}
