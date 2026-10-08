#include "static_asm.hpp"

using namespace static_asm::x86;
using namespace static_asm::x86::instructions;
using namespace static_asm::x86::registers;
using static_asm::core::assemble;

#ifndef STATIC_ASM_INVALID_CASE
    #error STATIC_ASM_INVALID_CASE must be defined
#endif

#if STATIC_ASM_INVALID_CASE == 0
constexpr auto valid = mov(qword_ptr(rbx + std::int8_t(0)), 1);
static_assert(valid.size() == 8);
constexpr auto valid_constant = mov(qword_ptr(rbx + disp<0>), 1);
static_assert(valid_constant.size() == 7);
constexpr auto valid_label = label<"valid_label">;
constexpr auto valid_symbolic = assemble(valid_label.assemble(jne(valid_label)));
static_assert(valid_symbolic.size() == 2);
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
constexpr auto target = label<"invalid_lea_target">;
constexpr auto invalid = assemble(lea(al, qword_ptr(target)), target.assemble(db(0)));
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
#elif STATIC_ASM_INVALID_CASE == 14
constexpr auto invalid = mov(eax, dword_ptr(rbx + disp<0x80000000LL>));
#elif STATIC_ASM_INVALID_CASE == 15
constexpr auto invalid = mov(eax, dword_ptr(rbx - disp<-0x80000000LL>));
#elif STATIC_ASM_INVALID_CASE == 16
constexpr auto invalid = mov(eax, dword_ptr(rbx + disp<1> + disp<2>));
#elif STATIC_ASM_INVALID_CASE == 17
constexpr auto missing = label<"missing">;
constexpr auto invalid = assemble(jmp(missing));
#elif STATIC_ASM_INVALID_CASE == 18
constexpr auto duplicate = label<"duplicate">;
constexpr auto invalid = assemble(duplicate.assemble(nop()), duplicate.assemble(ret()));
#elif STATIC_ASM_INVALID_CASE == 19
constexpr auto invalid = test(eax, qword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 20
constexpr auto invalid = test(ah, byte_ptr(r8 + disp<0>));
#elif STATIC_ASM_INVALID_CASE == 21
constexpr auto invalid = test(qword_ptr(rip + 0), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 22
constexpr auto invalid = test(qword_ptr(rip + 0), 0x80000000ULL);
#elif STATIC_ASM_INVALID_CASE == 23
constexpr auto invalid = test(qword_ptr(rbx), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 24
constexpr auto invalid = test(byte_ptr(rip + 0), 0x100);
#elif STATIC_ASM_INVALID_CASE == 25
constexpr auto invalid = test(word_ptr(rip + 0), 0x10000);
#elif STATIC_ASM_INVALID_CASE == 26
constexpr auto invalid = test(dword_ptr(rip + 0), 0x100000000ULL);
#elif STATIC_ASM_INVALID_CASE == 27
constexpr auto invalid = bsf(eax, qword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 28
constexpr auto invalid = bt(qword_ptr(rip + 0), ecx);
#elif STATIC_ASM_INVALID_CASE == 29
constexpr auto invalid = cmovz(eax, qword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 30
constexpr auto invalid = imul(eax, qword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 31
constexpr auto invalid = imul(eax, qword_ptr(rip + 0), imm8(1));
#elif STATIC_ASM_INVALID_CASE == 32
constexpr auto invalid = movzx(eax, dword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 33
constexpr auto invalid = movsxd(rax, qword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 34
constexpr auto invalid = call(dword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 35
constexpr auto invalid = jmp(dword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 36
constexpr auto invalid = push(dword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 37
constexpr auto invalid = pop(dword_ptr(rip + 0));
#elif STATIC_ASM_INVALID_CASE == 38
constexpr auto invalid = imul(rax, qword_ptr(rip + 0), imm16(1));
#elif STATIC_ASM_INVALID_CASE == 39
constexpr auto invalid = imul(rax, qword_ptr(rip + 0), 0x100000000ULL);
#else
    #error Unknown invalid operand case
#endif
