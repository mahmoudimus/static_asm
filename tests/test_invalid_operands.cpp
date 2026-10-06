#include "static_asm.hpp"

using namespace static_asm::x86;
using namespace static_asm::x86::instructions;
using namespace static_asm::x86::registers;

#ifndef STATIC_ASM_INVALID_CASE
#error STATIC_ASM_INVALID_CASE must be defined
#endif

#if STATIC_ASM_INVALID_CASE == 0
constexpr auto valid = mov(qword_ptr(rbx + std::int8_t(0)), 1);
static_assert(valid.size() == 8);
#elif STATIC_ASM_INVALID_CASE == 1
constexpr auto invalid = mov(eax, dword_ptr(rip + rcx * s4 + 16));
#elif STATIC_ASM_INVALID_CASE == 2
constexpr auto invalid = xchg(ah, byte_ptr(r8 + std::int8_t(0)));
#elif STATIC_ASM_INVALID_CASE == 3
constexpr auto invalid = xchg(ah, byte_ptr(r8));
#elif STATIC_ASM_INVALID_CASE == 4
constexpr auto invalid = xchg(eax, qword_ptr(rcx + std::int8_t(0)));
#elif STATIC_ASM_INVALID_CASE == 5
constexpr auto invalid = xchg(qword_ptr(rcx), eax);
#elif STATIC_ASM_INVALID_CASE == 6
constexpr auto invalid = build([](asm_block<>& b) {
    auto target = b.label();
    b.lea(al, target);
    b.bind(target);
    b.db(0);
});
#elif STATIC_ASM_INVALID_CASE == 7
constexpr auto invalid = mov(qword_ptr(rbx + std::int8_t(0)), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 8
constexpr auto invalid = mov(qword_ptr(rbx), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 9
constexpr auto invalid = mov(qword_ptr(rbx + std::int8_t(0)), 0x80000000ULL);
#elif STATIC_ASM_INVALID_CASE == 10
constexpr auto invalid = add(qword_ptr(rbx + std::int8_t(0)), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 11
constexpr auto invalid = add(qword_ptr(rbx), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 12
constexpr auto invalid = xchg(eax, rax);
#elif STATIC_ASM_INVALID_CASE == 13
constexpr auto invalid = xchg(ah, byte_ptr(rbx + r9 * s2));
#else
#error Unknown invalid operand case
#endif
