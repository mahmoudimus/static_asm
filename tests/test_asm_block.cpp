#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ---------------------------------------------------------------------------
// LOCK prefix wrapper
// ---------------------------------------------------------------------------
TEST(LockPrefix, PrependsF0) {
    constexpr auto a = lock_(add(dword_ptr(0x12121212), ecx));
    EXPECT_EQ(a, (internal::make_array<std::uint8_t>(0xF0, 0x01, 0x0C, 0x25, 0x12, 0x12, 0x12, 0x12)));

    constexpr auto b = lock_(add(qword_ptr(rbx + rcx * s4 + 0x20), rax));
    EXPECT_EQ(b, (internal::make_array<std::uint8_t>(0xF0, 0x48, 0x01, 0x84, 0x8B, 0x20, 0x00, 0x00, 0x00)));
}

// ---------------------------------------------------------------------------
// Base + displacement memory operands (no index) and memory-form unary ops.
// Encoded via the SIB path with index=none; verified against ndisasm.
// ---------------------------------------------------------------------------
TEST(MemoryBaseDisp, IncDecNegNot) {
    EXPECT_EQ(inc(qword_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x01)));
    EXPECT_EQ(inc(qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x44, 0x21, 0x20)));
    EXPECT_EQ(dec(dword_ptr(rax + std::int8_t(0x10))), (internal::make_array<std::uint8_t>(0xFF, 0x4C, 0x20, 0x10)));
    EXPECT_EQ(neg(qword_ptr(r8 + std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0x5C, 0x20, 0x08)));
    EXPECT_EQ(not_(qword_ptr(rbx - std::int8_t(0x4))), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x54, 0x23, 0xFC)));
    // disp32
    EXPECT_EQ(inc(qword_ptr(rcx + 0x12345678)), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x84, 0x21, 0x78, 0x56, 0x34, 0x12)));
}

TEST(MemoryBaseDisp, ComposesWithExistingInstructions) {
    // reg +/- disp now works for every memory-capable instruction
    EXPECT_EQ(add(rax, qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0x03, 0x44, 0x21, 0x20)));
    EXPECT_EQ(mov(rax, qword_ptr(rbp - std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x44, 0x25, 0xF8)));
}

TEST(MemoryBaseDisp, LockIncQwordMem) {
    // The end-to-end target: lock inc qword [rcx+0x20]
    EXPECT_EQ(lock_(inc(qword_ptr(rcx + std::int8_t(0x20)))),
        (internal::make_array<std::uint8_t>(0xF0, 0x48, 0xFF, 0x44, 0x21, 0x20)));
}

// ---------------------------------------------------------------------------
// Branch relaxation: short backward loop chooses rel8
// ---------------------------------------------------------------------------
TEST(AsmBlock, BackwardLoopRel8) {
    constexpr auto code = build([](asm_block<>& b) {
        auto loop = b.label();
        b.bind(loop);
        b.put(dec(ecx)); // FF C9
        b.jne(loop); // -> 75 FC (rel8, disp -4)
    });
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(0xFF, 0xC9, 0x75, 0xFC)));
}

// ---------------------------------------------------------------------------
// Branch relaxation: forward branch past +/-127 promotes to rel32
// ---------------------------------------------------------------------------
TEST(AsmBlock, ForwardBranchPromotesToRel32) {
    constexpr auto code = build([](asm_block<>& b) {
        auto end = b.label();
        b.jmp(end);
        for (int i = 0; i < 130; ++i) {
            b.put(nop());
        }
        b.bind(end);
    });
    EXPECT_EQ(code.size(), 5u + 130u); // 5-byte near jmp + 130 nops
    EXPECT_EQ(code[0], 0xE9); // E9 = near (rel32) jmp
    EXPECT_EQ(code[1], 0x82); // disp = 130
    EXPECT_EQ(code[2], 0x00);
}

// A branch that fits exactly stays rel8.
TEST(AsmBlock, ForwardBranchFitsRel8) {
    constexpr auto code = build([](asm_block<>& b) {
        auto end = b.label();
        b.jmp(end);
        for (int i = 0; i < 100; ++i) {
            b.put(nop());
        }
        b.bind(end);
    });
    EXPECT_EQ(code.size(), 2u + 100u);
    EXPECT_EQ(code[0], 0xEB); // EB = short jmp
    EXPECT_EQ(code[1], 100);
}

// ---------------------------------------------------------------------------
// CALL rel has no short form: always rel32
// ---------------------------------------------------------------------------
TEST(AsmBlock, CallAlwaysRel32) {
    constexpr auto code = build([](asm_block<>& b) {
        auto target = b.label();
        b.call(target);
        b.bind(target);
    });
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(0xE8, 0x00, 0x00, 0x00, 0x00)));
}

// ---------------------------------------------------------------------------
// Labels expose final byte offsets (runtime patch sites)
// ---------------------------------------------------------------------------
TEST(AsmBlock, LabelGivesPatchOffset) {
    constexpr std::size_t off = [] {
        asm_block<> b;
        auto ctx = b.label();
        b.put(nop()); // 1 leading byte
        b.bind(ctx);
        b.put(movabs(rax, 0)); // 48 B8 + imm64
        return b.offset_of(ctx);
    }();
    EXPECT_EQ(off, 1u); // movabs starts right after the nop
    EXPECT_EQ(off + 2, 3u); // imm64 slot sits after the 48 B8 opcode
}
