#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// Independent reference: NASM 64-bit mode, each operand uses [rel data].
TEST(RipCoverage, AllRemainingExplicitMemoryFamilies) {
    constexpr auto data = label<"rip_coverage_data">;
    constexpr auto code = core::assemble(
        bsf(rax, qword_ptr(data)),
        bsr(r9, qword_ptr(data)),
        bt(qword_ptr(data), r8),
        bts(qword_ptr(data), imm8(5)),
        cmovz(rax, qword_ptr(data)),
        imul(rax, qword_ptr(data)),
        imul(rax, qword_ptr(data), imm8(0xFD)),
        imul(r9, qword_ptr(data), imm32(0x1234)),
        movzx(eax, byte_ptr(data)),
        movsx(r9, word_ptr(data)),
        movsxd(r8, dword_ptr(data)),
        shl(qword_ptr(data)),
        shr(qword_ptr(data), cl),
        sar(qword_ptr(data), imm8(3)),
        call(qword_ptr(data)),
        jmp(qword_ptr(data)),
        push(qword_ptr(data)),
        pop(qword_ptr(data)),
        data.assemble(dq(0)));

    constexpr auto expected = internal::make_array<std::uint8_t>(
        0x48, 0x0F, 0xBC, 0x05, 0x80, 0x00, 0x00, 0x00,
        0x4C, 0x0F, 0xBD, 0x0D, 0x78, 0x00, 0x00, 0x00,
        0x4C, 0x0F, 0xA3, 0x05, 0x70, 0x00, 0x00, 0x00,
        0x48, 0x0F, 0xBA, 0x2D, 0x67, 0x00, 0x00, 0x00, 0x05,
        0x48, 0x0F, 0x44, 0x05, 0x5F, 0x00, 0x00, 0x00,
        0x48, 0x0F, 0xAF, 0x05, 0x57, 0x00, 0x00, 0x00,
        0x48, 0x6B, 0x05, 0x4F, 0x00, 0x00, 0x00, 0xFD,
        0x4C, 0x69, 0x0D, 0x44, 0x00, 0x00, 0x00, 0x34, 0x12, 0x00, 0x00,
        0x0F, 0xB6, 0x05, 0x3D, 0x00, 0x00, 0x00,
        0x4C, 0x0F, 0xBF, 0x0D, 0x35, 0x00, 0x00, 0x00,
        0x4C, 0x63, 0x05, 0x2E, 0x00, 0x00, 0x00,
        0x48, 0xD1, 0x25, 0x27, 0x00, 0x00, 0x00,
        0x48, 0xD3, 0x2D, 0x20, 0x00, 0x00, 0x00,
        0x48, 0xC1, 0x3D, 0x18, 0x00, 0x00, 0x00, 0x03,
        0xFF, 0x15, 0x12, 0x00, 0x00, 0x00,
        0xFF, 0x25, 0x0C, 0x00, 0x00, 0x00,
        0xFF, 0x35, 0x06, 0x00, 0x00, 0x00,
        0x8F, 0x05, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    EXPECT_EQ(code, expected);
    EXPECT_EQ(code.offset_of(data), 0x88u);
}

// NASM reference: mixed operand widths, aliases, extended destinations, and
// word-sized stack operands. All references resolve to the final data label.
TEST(RipCoverage, WidthsAndOpcodeVariants) {
    constexpr auto data = label<"rip_width_data">;
    constexpr auto code = core::assemble(
        bsf(ax, word_ptr(data)),
        bsr(r8d, dword_ptr(data)),
        btc(word_ptr(data), imm8(7)),
        btr(dword_ptr(data), ecx),
        cmovnz(r8d, dword_ptr(data)),
        imul(r9w, word_ptr(data), imm16(0x1234)),
        movzx(r8w, byte_ptr(data)),
        movsx(eax, word_ptr(data)),
        rol(byte_ptr(data)),
        rcr(word_ptr(data), cl),
        sal(dword_ptr(data), imm8(4)),
        push(word_ptr(data)),
        pop(word_ptr(data)),
        data.assemble(dq(0)));

    constexpr auto expected = internal::make_array<std::uint8_t>(
        0x66, 0x0F, 0xBC, 0x05, 0x5C, 0x00, 0x00, 0x00,
        0x44, 0x0F, 0xBD, 0x05, 0x54, 0x00, 0x00, 0x00,
        0x66, 0x0F, 0xBA, 0x3D, 0x4B, 0x00, 0x00, 0x00, 0x07,
        0x0F, 0xB3, 0x0D, 0x44, 0x00, 0x00, 0x00,
        0x44, 0x0F, 0x45, 0x05, 0x3C, 0x00, 0x00, 0x00,
        0x66, 0x44, 0x69, 0x0D, 0x32, 0x00, 0x00, 0x00, 0x34, 0x12,
        0x66, 0x44, 0x0F, 0xB6, 0x05, 0x29, 0x00, 0x00, 0x00,
        0x0F, 0xBF, 0x05, 0x22, 0x00, 0x00, 0x00,
        0xD0, 0x05, 0x1C, 0x00, 0x00, 0x00,
        0x66, 0xD3, 0x1D, 0x15, 0x00, 0x00, 0x00,
        0xC1, 0x25, 0x0E, 0x00, 0x00, 0x00, 0x04,
        0x66, 0xFF, 0x35, 0x07, 0x00, 0x00, 0x00,
        0x66, 0x8F, 0x05, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    EXPECT_EQ(code, expected);
    EXPECT_EQ(code.offset_of(data), 0x64u);
}

TEST(RipCoverage, SharedAddressTailHandlesExtendedBaseAndIndex) {
    constexpr auto memory = qword_ptr(r8 + r9 * s4 + disp<0x10>);
    constexpr auto bytes = core::assemble(
        bsf(r10, memory),
        bt(memory, r11),
        cmovz(r10, memory),
        imul(r10, memory, imm8(0xFF)),
        movzx(r10d, byte_ptr(r8 + r9 * s4 + disp<0x10>)),
        sar(memory, cl),
        call(memory),
        push(memory));
    constexpr auto expected = internal::make_array<std::uint8_t>(
        0x4F, 0x0F, 0xBC, 0x54, 0x88, 0x10,
        0x4F, 0x0F, 0xA3, 0x5C, 0x88, 0x10,
        0x4F, 0x0F, 0x44, 0x54, 0x88, 0x10,
        0x4F, 0x6B, 0x54, 0x88, 0x10, 0xFF,
        0x47, 0x0F, 0xB6, 0x54, 0x88, 0x10,
        0x4B, 0xD3, 0x7C, 0x88, 0x10,
        0x43, 0xFF, 0x54, 0x88, 0x10,
        0x43, 0xFF, 0x74, 0x88, 0x10);
    EXPECT_EQ(bytes, expected);
}

TEST(RipCoverage, ConditionalAndShiftAliasesAcceptLabeledMemory) {
    constexpr auto data = label<"rip_alias_data">;
    constexpr auto cmov_refs = std::array{
        cmova(rax, qword_ptr(data)), cmovnbe(rax, qword_ptr(data)),
        cmovae(rax, qword_ptr(data)), cmovnb(rax, qword_ptr(data)), cmovnc(rax, qword_ptr(data)),
        cmovb(rax, qword_ptr(data)), cmovc(rax, qword_ptr(data)), cmovnae(rax, qword_ptr(data)),
        cmovbe(rax, qword_ptr(data)), cmovna(rax, qword_ptr(data)),
        cmove(rax, qword_ptr(data)), cmovz(rax, qword_ptr(data)),
        cmovg(rax, qword_ptr(data)), cmovnle(rax, qword_ptr(data)),
        cmovge(rax, qword_ptr(data)), cmovnl(rax, qword_ptr(data)),
        cmovl(rax, qword_ptr(data)), cmovnge(rax, qword_ptr(data)),
        cmovle(rax, qword_ptr(data)), cmovng(rax, qword_ptr(data)),
        cmovne(rax, qword_ptr(data)), cmovnz(rax, qword_ptr(data)),
        cmovno(rax, qword_ptr(data)), cmovnp(rax, qword_ptr(data)), cmovpo(rax, qword_ptr(data)),
        cmovns(rax, qword_ptr(data)), cmovo(rax, qword_ptr(data)),
        cmovp(rax, qword_ptr(data)), cmovpe(rax, qword_ptr(data)), cmovs(rax, qword_ptr(data))
    };
    constexpr auto shifts_by_one = std::array{
        shl(qword_ptr(data)), shr(qword_ptr(data)), sal(qword_ptr(data)), sar(qword_ptr(data)),
        rol(qword_ptr(data)), ror(qword_ptr(data)), rcl(qword_ptr(data)), rcr(qword_ptr(data))
    };
    constexpr auto shifts_by_cl = std::array{
        shl(qword_ptr(data), cl), shr(qword_ptr(data), cl),
        sal(qword_ptr(data), cl), sar(qword_ptr(data), cl),
        rol(qword_ptr(data), cl), ror(qword_ptr(data), cl),
        rcl(qword_ptr(data), cl), rcr(qword_ptr(data), cl)
    };
    constexpr auto shifts_by_immediate = std::array{
        shl(qword_ptr(data), imm8(3)), shr(qword_ptr(data), imm8(3)),
        sal(qword_ptr(data), imm8(3)), sar(qword_ptr(data), imm8(3)),
        rol(qword_ptr(data), imm8(3)), ror(qword_ptr(data), imm8(3)),
        rcl(qword_ptr(data), imm8(3)), rcr(qword_ptr(data), imm8(3))
    };
    constexpr auto bit_test_register = std::array{
        bt(qword_ptr(data), rax), btc(qword_ptr(data), rax),
        btr(qword_ptr(data), rax), bts(qword_ptr(data), rax)
    };
    constexpr auto bit_test_immediate = std::array{
        bt(qword_ptr(data), imm8(3)), btc(qword_ptr(data), imm8(3)),
        btr(qword_ptr(data), imm8(3)), bts(qword_ptr(data), imm8(3))
    };
    static_assert(cmov_refs.size() == 30);
    static_assert(shifts_by_one.size() == 8 && shifts_by_cl.size() == 8 && shifts_by_immediate.size() == 8);
    static_assert(bit_test_register.size() == 4 && bit_test_immediate.size() == 4);
    EXPECT_EQ(cmov_refs[0].bytes[2], 0x47);
    EXPECT_EQ(shifts_by_one[0].bytes[1], 0xD1);
    EXPECT_EQ(bit_test_register[0].bytes[2], 0xA3);
}
