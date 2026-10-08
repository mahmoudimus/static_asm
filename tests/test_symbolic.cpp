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
    EXPECT_EQ(inc(qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x41, 0x20)));
    EXPECT_EQ(dec(dword_ptr(rax + std::int8_t(0x10))), (internal::make_array<std::uint8_t>(0xFF, 0x48, 0x10)));
    EXPECT_EQ(neg(qword_ptr(r8 + std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0x58, 0x08)));
    EXPECT_EQ(not_(qword_ptr(rbx - std::int8_t(0x4))), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x53, 0xFC)));
    // disp32
    EXPECT_EQ(inc(qword_ptr(rcx + 0x12345678)), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x81, 0x78, 0x56, 0x34, 0x12)));
}

TEST(MemoryBaseDisp, ComposesWithExistingInstructions) {
    // reg +/- disp now works for every memory-capable instruction
    EXPECT_EQ(add(rax, qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0x03, 0x41, 0x20)));
    EXPECT_EQ(mov(rax, qword_ptr(rbp - std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x48, 0x8B, 0x45, 0xF8)));
}

TEST(MemoryBaseDisp, RspBaseKeepsSib) {
    // RSP/R12 base requires a SIB byte (r/m=100 means "SIB follows")
    EXPECT_EQ(inc(qword_ptr(rsp + std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x48, 0xFF, 0x44, 0x24, 0x08)));
}

TEST(MemoryBaseDisp, MulDivGroup) {
    EXPECT_EQ(mul(qword_ptr(rcx)), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x21)));
    EXPECT_EQ(mul(qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x61, 0x20)));
    EXPECT_EQ(imul(qword_ptr(rcx + std::int8_t(0x20))), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0x69, 0x20)));
    EXPECT_EQ(div(dword_ptr(rax + std::int8_t(0x10))), (internal::make_array<std::uint8_t>(0xF7, 0x70, 0x10)));
    EXPECT_EQ(idiv(qword_ptr(r8 + std::int8_t(0x8))), (internal::make_array<std::uint8_t>(0x49, 0xF7, 0x78, 0x08)));
    EXPECT_EQ(div(qword_ptr(rcx + 0x12345678)), (internal::make_array<std::uint8_t>(0x48, 0xF7, 0xB1, 0x78, 0x56, 0x34, 0x12)));
}

TEST(MemoryBaseDisp, LockIncQwordMem) {
    // The end-to-end target: lock inc qword [rcx+0x20]
    EXPECT_EQ(lock_(inc(qword_ptr(rcx + std::int8_t(0x20)))),
        (internal::make_array<std::uint8_t>(0xF0, 0x48, 0xFF, 0x41, 0x20)));
}

// Symbolic labels are resolved by the outer core::assemble call.
TEST(SymbolicAssembly, BackwardLoop) {
    constexpr auto loop = label<"loop">;
    constexpr auto code = core::assemble(loop.assemble(dec(ecx), jne(loop)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(0xFF, 0xC9, 0x75, 0xFC)));
    EXPECT_EQ(code.offset_of(loop), 0u);
}

TEST(SymbolicAssembly, ForwardBranchRelaxesAcrossFragments) {
    constexpr auto end = label<"end">;
    constexpr auto padding = [] {
        std::array<std::uint8_t, 130> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto code = core::assemble(jmp(end), padding, end.assemble(ret()));
    EXPECT_EQ(code.size(), 136u);
    EXPECT_EQ(code[0], 0xE9);
    EXPECT_EQ(code[1], 0x82);
    EXPECT_EQ(code.offset_of(end), 135u);
    EXPECT_EQ(code[135], 0xC3);
}

TEST(SymbolicAssembly, NestedFragmentAndComposition) {
    constexpr auto loop = label<"nested_loop">;
    constexpr auto done = label<"nested_done">;
    constexpr auto code = core::assemble(
        loop.assemble(dec(ecx), jne(loop), jmp(done)),
        nop(),
        done.assemble(ret()));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(0xFF, 0xC9, 0x75, 0xFC, 0xEB, 0x01, 0x90, 0xC3)));
    EXPECT_EQ(code.offset_of(done), 7u);
    constexpr auto with_trailer = core::assemble(code, nop());
    EXPECT_EQ(with_trailer.size(), 9u);
    EXPECT_EQ(with_trailer[8], 0x90);
}

TEST(SymbolicAssembly, NestedLabelDefinitions) {
    constexpr auto outer = label<"outer">;
    constexpr auto inner = label<"inner">;
    constexpr auto code = core::assemble(outer.assemble(nop(), inner.assemble(jmp(outer))), ret());
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(0x90, 0xEB, 0xFD, 0xC3)));
    EXPECT_EQ(code.offset_of(inner), 1u);
}

TEST(SymbolicAssembly, RipRelativeLabeledData) {
    constexpr auto data = label<"data">;
    constexpr auto code = core::assemble(
        lea(rax, qword_ptr(data)),
        ret(),
        data.assemble(dq(0xCAFEBABEULL)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0x8D, 0x05, 0x01, 0x00, 0x00, 0x00, 0xC3,
                        0xBE, 0xBA, 0xFE, 0xCA, 0x00, 0x00, 0x00, 0x00)));
    EXPECT_EQ(code.offset_of(data), 8u);
}

TEST(SymbolicAssembly, RipImmediateTailAndBackwardReference) {
    constexpr auto data = label<"immediate_data">;
    constexpr auto code = core::assemble(
        data.assemble(dd(0)),
        mov(dword_ptr(data), 0x7B),
        mov(eax, dword_ptr(data)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x00, 0x00, 0x00, 0x00,
                        0xC7, 0x05, 0xF2, 0xFF, 0xFF, 0xFF, 0x7B, 0x00, 0x00, 0x00,
                        0x8B, 0x05, 0xEC, 0xFF, 0xFF, 0xFF)));
}

TEST(SymbolicAssembly, ShortForwardBranchAndCall) {
    constexpr auto end = label<"short_end">;
    constexpr auto padding = [] {
        std::array<std::uint8_t, 100> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto short_code = core::assemble(jmp(end), padding, end.assemble());
    EXPECT_EQ(short_code.size(), 102u);
    EXPECT_EQ(short_code[0], 0xEB);
    EXPECT_EQ(short_code[1], 100);

    constexpr auto target = label<"call_target">;
    constexpr auto call_code = core::assemble(call(target), target.assemble(ret()));
    EXPECT_EQ(call_code, (internal::make_array<std::uint8_t>(0xE8, 0x00, 0x00, 0x00, 0x00, 0xC3)));
}

TEST(SymbolicAssembly, BranchRel8Boundary) {
    constexpr auto end = label<"boundary_end">;
    constexpr auto short_padding = [] {
        std::array<std::uint8_t, 127> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto long_padding = [] {
        std::array<std::uint8_t, 128> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto short_code = core::assemble(jmp(end), short_padding, end.assemble());
    constexpr auto long_code = core::assemble(jmp(end), long_padding, end.assemble());
    EXPECT_EQ(short_code[0], 0xEB);
    EXPECT_EQ(short_code[1], 0x7F);
    EXPECT_EQ(long_code[0], 0xE9);
    EXPECT_EQ(long_code[1], 0x80);
}

TEST(SymbolicAssembly, IterativeBranchRelaxation) {
    constexpr auto first = label<"first">;
    constexpr auto far = label<"far">;
    constexpr auto near_padding = [] {
        std::array<std::uint8_t, 120> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto far_padding = [] {
        std::array<std::uint8_t, 130> bytes{};
        bytes.fill(0x90);
        return bytes;
    }();
    constexpr auto code = core::assemble(
        jmp(first), near_padding, jmp(far),
        nop(), nop(), nop(), nop(), nop(),
        first.assemble(nop()), far_padding, far.assemble(ret()));
    EXPECT_EQ(code[0], 0xE9); // The second branch growing pushes this one out of rel8 range.
    EXPECT_EQ(code[125], 0xE9); // The second branch was already out of range.
    EXPECT_EQ(code.offset_of(first), 135u);
}

TEST(SymbolicAssembly, DataAndRuntimePatchOffset) {
    constexpr auto slot = label<"patch_slot">;
    constexpr auto code = core::assemble(nop(), slot.assemble(movabs(rax, 0)), db(0xAB), dw(0x1234), dd(0xDEADBEEF));
    EXPECT_EQ(code.offset_of(slot), 1u);
    EXPECT_EQ(code.offset_of(slot) + 2, 3u);
    EXPECT_EQ(code[11], 0xAB);
    EXPECT_EQ(code[12], 0x34);
    EXPECT_EQ(code[13], 0x12);
    EXPECT_EQ(code[14], 0xEF);
}

TEST(SymbolicAssembly, TypedMemoryForms) {
    constexpr auto data = label<"typed_data">;
    constexpr auto code = core::assemble(
        lea(rax, qword_ptr(data)),
        mov(ecx, dword_ptr(data)),
        mov(qword_ptr(data), rbx),
        ret(),
        data.assemble(dq(0)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0x8D, 0x05, 0x0E, 0x00, 0x00, 0x00,
                        0x8B, 0x0D, 0x08, 0x00, 0x00, 0x00,
                        0x48, 0x89, 0x1D, 0x01, 0x00, 0x00, 0x00,
                        0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)));
}

TEST(SymbolicAssembly, SupportedRipMemoryFamilies) {
    constexpr auto data = label<"family_data">;
    constexpr auto code = core::assemble(
        add(rax, qword_ptr(data)),
        sub(qword_ptr(data), rbx),
        cmp(rcx, qword_ptr(data)),
        inc(qword_ptr(data)),
        neg(qword_ptr(data)),
        mul(qword_ptr(data)),
        xchg(qword_ptr(data), rdx),
        and_(qword_ptr(data), 0x10),
        ret(),
        data.assemble(dq(0)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0x03, 0x05, 0x36, 0x00, 0x00, 0x00,
                        0x48, 0x29, 0x1D, 0x2F, 0x00, 0x00, 0x00,
                        0x48, 0x3B, 0x0D, 0x28, 0x00, 0x00, 0x00,
                        0x48, 0xFF, 0x05, 0x21, 0x00, 0x00, 0x00,
                        0x48, 0xF7, 0x1D, 0x1A, 0x00, 0x00, 0x00,
                        0x48, 0xF7, 0x25, 0x13, 0x00, 0x00, 0x00,
                        0x48, 0x87, 0x15, 0x0C, 0x00, 0x00, 0x00,
                        0x48, 0x81, 0x25, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
                        0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)));
}

TEST(SymbolicAssembly, TestRegisterWithLabeledMemory) {
    constexpr auto data = label<"test_data">;
    constexpr auto code = core::assemble(
        test(rax, qword_ptr(data)),
        ret(),
        data.assemble(dq(0)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0x85, 0x05, 0x01, 0x00, 0x00, 0x00,
                        0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)));
}

TEST(SymbolicAssembly, TestImmediateWithLabeledMemory) {
    constexpr auto data = label<"test_immediate_data">;
    constexpr auto code = core::assemble(
        test(qword_ptr(data), 0x10),
        ret(),
        data.assemble(dq(0)));
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0xF7, 0x05, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xC3,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)));
}
