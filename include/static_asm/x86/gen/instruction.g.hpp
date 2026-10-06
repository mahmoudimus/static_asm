// =============================================================================
// GENERATED FILE - DO NOT EDIT MANUALLY
// =============================================================================
// Source:    scripts/x86reference.xml
// MD5:       7f3fd7154e0809defc1a14de3d680bf8
// Generated: 2026-01-25T01:25:01Z
// =============================================================================
#pragma once

#include <array>
#include <cstdint>

#include "../encoder.hpp"
#include "../instruction_db.hpp"

namespace static_asm::x86::instructions {

    template<typename Op1, typename Op2>
    inline constexpr auto adc(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::adc>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto add(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::add>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto and_(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::and_>(op1, op2);
    }

    // BSF - Bit Scan Forward
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto bsf(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::bsf>(op1, op2);
    }

    // BSR - Bit Scan Reverse
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto bsr(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::bsr>(op1, op2);
    }

    // BSWAP - Byte Swap (32-bit or 64-bit register only)
    template<typename Op1>
        requires Register<Op1> && (Op1::size == 32 || Op1::size == 64)
    inline constexpr auto bswap(const Op1& op1) {
        return encode<e_instruction_id::bswap>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto bt(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::bt>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto btc(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::btc>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto btr(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::btr>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto bts(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::bts>(op1, op2);
    }

    template<typename Op1>
    inline constexpr auto call(const Op1& op1) {
        return encode<e_instruction_id::call>(op1);
    }

    // ==========================================================================
    // CMOV - Conditional Move instructions
    // ==========================================================================

    // CMOVA/CMOVNBE - Move if above (CF=0 and ZF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmova(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmova>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnbe(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmova>(op1, op2); // Alias for CMOVA
    }

    // CMOVAE/CMOVNB/CMOVNC - Move if above or equal (CF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovae(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovae>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnb(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovae>(op1, op2); // Alias for CMOVAE
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnc(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovae>(op1, op2); // Alias for CMOVAE
    }

    // CMOVB/CMOVC/CMOVNAE - Move if below (CF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovb(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovb>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovc(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovb>(op1, op2); // Alias for CMOVB
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnae(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovb>(op1, op2); // Alias for CMOVB
    }

    // CMOVBE/CMOVNA - Move if below or equal (CF=1 or ZF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovbe(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovbe>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovna(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovbe>(op1, op2); // Alias for CMOVBE
    }

    // CMOVE/CMOVZ - Move if equal (ZF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmove(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmove>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovz(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmove>(op1, op2); // Alias for CMOVE
    }

    // CMOVG/CMOVNLE - Move if greater (ZF=0 and SF=OF)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovg(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovg>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnle(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovg>(op1, op2); // Alias for CMOVG
    }

    // CMOVGE/CMOVNL - Move if greater or equal (SF=OF)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovge(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovge>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnl(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovge>(op1, op2); // Alias for CMOVGE
    }

    // CMOVL/CMOVNGE - Move if less (SF!=OF)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovl(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovl>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnge(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovl>(op1, op2); // Alias for CMOVL
    }

    // CMOVLE/CMOVNG - Move if less or equal (ZF=1 or SF!=OF)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovle(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovle>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovng(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovle>(op1, op2); // Alias for CMOVLE
    }

    // CMOVNE/CMOVNZ - Move if not equal (ZF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovne(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovnz>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnz(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovnz>(op1, op2);
    }

    // CMOVNO - Move if not overflow (OF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovno(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovno>(op1, op2);
    }

    // CMOVNP/CMOVPO - Move if not parity (PF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovnp(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovnp>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovpo(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovnp>(op1, op2); // Alias for CMOVNP
    }

    // CMOVNS - Move if not sign (SF=0)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovns(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovns>(op1, op2);
    }

    // CMOVO - Move if overflow (OF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovo(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovo>(op1, op2);
    }

    // CMOVP/CMOVPE - Move if parity (PF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovp(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovp>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovpe(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovp>(op1, op2); // Alias for CMOVP
    }

    // CMOVS - Move if sign (SF=1)
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto cmovs(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmovs>(op1, op2);
    }

    // ==========================================================================
    // End of CMOV instructions
    // ==========================================================================

    template<typename Op1, typename Op2>
    inline constexpr auto cmp(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::cmp>(op1, op2);
    }

    template<typename Op1>
    inline constexpr auto dec(const Op1& op1) {
        return encode<e_instruction_id::dec>(op1);
    }

    template<typename Op1>
    inline constexpr auto div(const Op1& op1) {
        return encode<e_instruction_id::div>(op1);
    }

    template<typename Op1>
    inline constexpr auto idiv(const Op1& op1) {
        return encode<e_instruction_id::idiv>(op1);
    }

    // IMUL one-operand form: rdx:rax = rax * op1
    template<typename Op1>
    inline constexpr auto imul(const Op1& op1) {
        return encode<e_instruction_id::imul>(op1);
    }

    // IMUL two-operand form: op1 = op1 * op2
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto imul(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::imul_two>(op1, op2);
    }

    // IMUL three-operand form: op1 = op2 * op3 (with immediate operand)
    template<typename Op1, typename Op2, typename Op3>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>) && Immediate<Op3>
    inline constexpr auto imul(const Op1& op1, const Op2& op2, const Op3& op3) {
        return encode<e_instruction_id::imul_three>(op1, op2, op3);
    }

    // IMUL three-operand form with integer literal
    template<typename Op1, typename Op2, typename Op3>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>) && Integer<Op3>
    inline constexpr auto imul(const Op1& op1, const Op2& op2, const Op3& op3) {
        auto imm = imm32(static_cast<std::uint32_t>(op3));
        return encode<e_instruction_id::imul_three>(op1, op2, imm);
    }

    // IMUL three-operand form with imm8 - explicit overload for smaller encoding
    template<typename Op1, typename Op2>
        requires Register<Op1> && (Register<Op2> || Memory<Op2>)
    inline constexpr auto imul(const Op1& op1, const Op2& op2, const imm8& op3) {
        return encode<e_instruction_id::imul_three>(op1, op2, op3);
    }

    template<typename Op1>
    inline constexpr auto inc(const Op1& op1) {
        return encode<e_instruction_id::inc>(op1);
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jb(Address address) {
        return encode<e_instruction_id::jb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jb(Address address) {
        return jb(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnae(Address address) {
        return encode<e_instruction_id::jb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnae(Address address) {
        return jnae(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jc(Address address) {
        return encode<e_instruction_id::jb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jc(Address address) {
        return jc(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jbe(Address address) {
        return encode<e_instruction_id::jbe>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jbe(Address address) {
        return jbe(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jna(Address address) {
        return encode<e_instruction_id::jbe>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jna(Address address) {
        return jna(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jl(Address address) {
        return encode<e_instruction_id::jl>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jl(Address address) {
        return jl(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnge(Address address) {
        return encode<e_instruction_id::jl>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnge(Address address) {
        return jnge(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jle(Address address) {
        return encode<e_instruction_id::jle>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jle(Address address) {
        return jle(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jng(Address address) {
        return encode<e_instruction_id::jle>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jng(Address address) {
        return jng(imm8(address));
    }

    template<typename Address>
        requires(Immediate<Address> || Register<Address> || (Memory<Address> && Register<typename Address::value_type>))
    inline constexpr auto jmp(Address address) {
        return encode<e_instruction_id::jmp>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jmp(Address address) {
        return jmp(imm32(address));
    }

    // jmp(here) - infinite loop, encodes as EB FE (jmp rel8 -2)
    // This is the idiomatic way to express "jmp $" (jump to current instruction)
    template<typename T>
        requires IsHere<T>
    inline constexpr auto jmp([[maybe_unused]] T) {
        return internal::make_array<std::uint8_t>(0xEB, 0xFE);
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnb(Address address) {
        return encode<e_instruction_id::jnb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnb(Address address) {
        return jnb(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jae(Address address) {
        return encode<e_instruction_id::jnb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jae(Address address) {
        return jae(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnc(Address address) {
        return encode<e_instruction_id::jnb>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnc(Address address) {
        return jnc(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnbe(Address address) {
        return encode<e_instruction_id::jnbe>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnbe(Address address) {
        return jnbe(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto ja(Address address) {
        return encode<e_instruction_id::jnbe>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto ja(Address address) {
        return ja(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnl(Address address) {
        return encode<e_instruction_id::jnl>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnl(Address address) {
        return jnl(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jge(Address address) {
        return encode<e_instruction_id::jnl>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jge(Address address) {
        return jge(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnle(Address address) {
        return encode<e_instruction_id::jnle>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnle(Address address) {
        return jnle(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jg(Address address) {
        return encode<e_instruction_id::jnle>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jg(Address address) {
        return jg(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jno(Address address) {
        return encode<e_instruction_id::jno>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jno(Address address) {
        return jno(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnp(Address address) {
        return encode<e_instruction_id::jnp>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnp(Address address) {
        return jnp(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jpo(Address address) {
        return encode<e_instruction_id::jnp>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jpo(Address address) {
        return jpo(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jns(Address address) {
        return encode<e_instruction_id::jns>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jns(Address address) {
        return jns(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jnz(Address address) {
        return encode<e_instruction_id::jnz>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnz(Address address) {
        return jnz(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jne(Address address) {
        return encode<e_instruction_id::jnz>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jne(Address address) {
        return jne(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jo(Address address) {
        return encode<e_instruction_id::jo>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jo(Address address) {
        return jo(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jp(Address address) {
        return encode<e_instruction_id::jp>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jp(Address address) {
        return jp(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jpe(Address address) {
        return encode<e_instruction_id::jp>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jpe(Address address) {
        return jpe(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto js(Address address) {
        return encode<e_instruction_id::js>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto js(Address address) {
        return js(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto jz(Address address) {
        return encode<e_instruction_id::jz>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jz(Address address) {
        return jz(imm8(address));
    }

    template<typename Address>
        requires Immediate8<Address>
    inline constexpr auto je(Address address) {
        return encode<e_instruction_id::jz>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto je(Address address) {
        return je(imm8(address));
    }

    // =========================================================================
    // Near conditional jumps (rel32) - 0F 8x opcodes
    // =========================================================================

    // JO near (0F 80)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jo_near(Address address) {
        return encode<e_instruction_id::jo_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jo_near(Address address) {
        return jo_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JNO near (0F 81)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jno_near(Address address) {
        return encode<e_instruction_id::jno_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jno_near(Address address) {
        return jno_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JB/JC/JNAE near (0F 82)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jb_near(Address address) {
        return encode<e_instruction_id::jb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jb_near(Address address) {
        return jb_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jc_near(Address address) {
        return encode<e_instruction_id::jb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jc_near(Address address) {
        return jc_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnae_near(Address address) {
        return encode<e_instruction_id::jb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnae_near(Address address) {
        return jnae_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JAE/JNB/JNC near (0F 83)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnb_near(Address address) {
        return encode<e_instruction_id::jnb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnb_near(Address address) {
        return jnb_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jae_near(Address address) {
        return encode<e_instruction_id::jnb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jae_near(Address address) {
        return jae_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnc_near(Address address) {
        return encode<e_instruction_id::jnb_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnc_near(Address address) {
        return jnc_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JE/JZ near (0F 84)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jz_near(Address address) {
        return encode<e_instruction_id::jz_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jz_near(Address address) {
        return jz_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto je_near(Address address) {
        return encode<e_instruction_id::jz_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto je_near(Address address) {
        return je_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JNE/JNZ near (0F 85)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnz_near(Address address) {
        return encode<e_instruction_id::jnz_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnz_near(Address address) {
        return jnz_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jne_near(Address address) {
        return encode<e_instruction_id::jnz_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jne_near(Address address) {
        return jne_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JBE/JNA near (0F 86)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jbe_near(Address address) {
        return encode<e_instruction_id::jbe_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jbe_near(Address address) {
        return jbe_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jna_near(Address address) {
        return encode<e_instruction_id::jbe_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jna_near(Address address) {
        return jna_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JA/JNBE near (0F 87)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnbe_near(Address address) {
        return encode<e_instruction_id::jnbe_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnbe_near(Address address) {
        return jnbe_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto ja_near(Address address) {
        return encode<e_instruction_id::jnbe_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto ja_near(Address address) {
        return ja_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JS near (0F 88)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto js_near(Address address) {
        return encode<e_instruction_id::js_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto js_near(Address address) {
        return js_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JNS near (0F 89)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jns_near(Address address) {
        return encode<e_instruction_id::jns_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jns_near(Address address) {
        return jns_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JP/JPE near (0F 8A)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jp_near(Address address) {
        return encode<e_instruction_id::jp_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jp_near(Address address) {
        return jp_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jpe_near(Address address) {
        return encode<e_instruction_id::jp_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jpe_near(Address address) {
        return jpe_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JNP/JPO near (0F 8B)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnp_near(Address address) {
        return encode<e_instruction_id::jnp_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnp_near(Address address) {
        return jnp_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jpo_near(Address address) {
        return encode<e_instruction_id::jnp_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jpo_near(Address address) {
        return jpo_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JL/JNGE near (0F 8C)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jl_near(Address address) {
        return encode<e_instruction_id::jl_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jl_near(Address address) {
        return jl_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnge_near(Address address) {
        return encode<e_instruction_id::jl_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnge_near(Address address) {
        return jnge_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JGE/JNL near (0F 8D)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnl_near(Address address) {
        return encode<e_instruction_id::jnl_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnl_near(Address address) {
        return jnl_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jge_near(Address address) {
        return encode<e_instruction_id::jnl_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jge_near(Address address) {
        return jge_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JLE/JNG near (0F 8E)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jle_near(Address address) {
        return encode<e_instruction_id::jle_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jle_near(Address address) {
        return jle_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jng_near(Address address) {
        return encode<e_instruction_id::jle_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jng_near(Address address) {
        return jng_near(imm32(static_cast<std::uint32_t>(address)));
    }

    // JG/JNLE near (0F 8F)
    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jnle_near(Address address) {
        return encode<e_instruction_id::jnle_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jnle_near(Address address) {
        return jnle_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Address>
        requires std::same_as<Address, imm32>
    inline constexpr auto jg_near(Address address) {
        return encode<e_instruction_id::jnle_near>(address);
    }

    template<typename Address>
        requires Integer<Address>
    inline constexpr auto jg_near(Address address) {
        return jg_near(imm32(static_cast<std::uint32_t>(address)));
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && Memory<Op2>
    inline constexpr auto lea(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::lea>(op1, op2);
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && SIBMemory<Op2>
    inline constexpr auto lea(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::lea>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto mov(const Op1& op1, const Op2& op2) {
        // For 64-bit registers with integer immediates that don't fit in 32 bits,
        // automatically use movabs encoding (mov r64, imm64)
        if constexpr (Register64<Op1> && Integer<Op2>) {
            // Check if value fits in signed 32-bit (for sign-extended mov r64, imm32)
            constexpr bool needs_64bit = sizeof(Op2) > 4;
            if constexpr (needs_64bit) {
                return encode<e_instruction_id::movabs>(op1, op2);
            } else {
                return encode<e_instruction_id::mov>(op1, op2);
            }
        } else if constexpr (Register64<Op1> && Immediate64<Op2>) {
            // Explicit 64-bit immediate type
            return encode<e_instruction_id::movabs>(op1, op2);
        } else {
            return encode<e_instruction_id::mov>(op1, op2);
        }
    }

    // Explicit movabs for when you always want 64-bit immediate encoding
    template<typename Op1, typename Op2>
    inline constexpr auto movabs(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::movabs>(op1, op2);
    }

    template<typename Op1>
    inline constexpr auto mul(const Op1& op1) {
        return encode<e_instruction_id::mul>(op1);
    }

    template<typename Op1>
    inline constexpr auto neg(const Op1& op1) {
        return encode<e_instruction_id::neg>(op1);
    }

    inline constexpr auto nop() {
        return encode<e_instruction_id::nop>();
    }

    template<int Len>
    inline constexpr auto nop() {
        return encode_nop<Len>();
    }

#define NOP(len) nop<len>()

    template<typename Op1>
    inline constexpr auto not_(const Op1& op1) {
        return encode<e_instruction_id::not_>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto or_(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::or_>(op1, op2);
    }

    template<typename Op1>
    inline constexpr auto pop(const Op1& op1) {
        return encode<e_instruction_id::pop>(op1);
    }

    template<typename Op1>
    inline constexpr auto push(const Op1& op1) {
        return encode<e_instruction_id::push>(op1);
    }

    template<typename Op1>
    inline constexpr auto ret(const Op1& op1) {
        return encode<e_instruction_id::ret>(op1);
    }

    inline constexpr auto ret() {
        return encode<e_instruction_id::ret>();
    }

    template<typename Op1>
    inline constexpr auto retf(const Op1& op1) {
        return encode<e_instruction_id::retf>(op1);
    }

    inline constexpr auto retf() {
        return encode<e_instruction_id::retf>();
    }

    template<typename Op1, typename Op2>
    inline constexpr auto sbb(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::sbb>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto sub(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::sub>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto test(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::test>(op1, op2);
    }

    inline constexpr auto ud2() {
        return encode<e_instruction_id::ud2>();
    }

    template<typename Op1, typename Op2>
        requires Register<Op1> && Register<Op2>
    inline constexpr auto xchg(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::xchg>(op1, op2);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto xor_(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::xor_>(op1, op2);
    }

    // Shift/rotate instructions
    // SHL - Shift Left (Logical)
    template<typename Op1>
    inline constexpr auto shl(const Op1& op1) {
        return encode<e_instruction_id::shl>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto shl(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::shl>(op1, op2);
    }

    // SHR - Shift Right (Logical)
    template<typename Op1>
    inline constexpr auto shr(const Op1& op1) {
        return encode<e_instruction_id::shr>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto shr(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::shr>(op1, op2);
    }

    // SAL - Shift Arithmetic Left (same as SHL)
    template<typename Op1>
    inline constexpr auto sal(const Op1& op1) {
        return encode<e_instruction_id::sal>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto sal(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::sal>(op1, op2);
    }

    // SAR - Shift Arithmetic Right
    template<typename Op1>
    inline constexpr auto sar(const Op1& op1) {
        return encode<e_instruction_id::sar>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto sar(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::sar>(op1, op2);
    }

    // ROL - Rotate Left
    template<typename Op1>
    inline constexpr auto rol(const Op1& op1) {
        return encode<e_instruction_id::rol>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto rol(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::rol>(op1, op2);
    }

    // ROR - Rotate Right
    template<typename Op1>
    inline constexpr auto ror(const Op1& op1) {
        return encode<e_instruction_id::ror>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto ror(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::ror>(op1, op2);
    }

    // RCL - Rotate through Carry Left
    template<typename Op1>
    inline constexpr auto rcl(const Op1& op1) {
        return encode<e_instruction_id::rcl>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto rcl(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::rcl>(op1, op2);
    }

    // RCR - Rotate through Carry Right
    template<typename Op1>
    inline constexpr auto rcr(const Op1& op1) {
        return encode<e_instruction_id::rcr>(op1);
    }

    template<typename Op1, typename Op2>
    inline constexpr auto rcr(const Op1& op1, const Op2& op2) {
        return encode<e_instruction_id::rcr>(op1, op2);
    }

    // ==========================================================================
    // String Instructions
    // ==========================================================================

    // MOVS - Move String (copy from [rsi] to [rdi])
    inline constexpr auto movsb() {
        return encode<e_instruction_id::movsb>();
    }

    inline constexpr auto movsw() {
        return encode<e_instruction_id::movsw>();
    }

    inline constexpr auto movsd_() {
        // Note: Using movsd_() to avoid conflict with SSE movsd
        return encode<e_instruction_id::movsd>();
    }

    inline constexpr auto movsq() {
        return encode_string_q<e_instruction_id::movsq>();
    }

    // CMPS - Compare String ([rsi] with [rdi])
    inline constexpr auto cmpsb() {
        return encode<e_instruction_id::cmpsb>();
    }

    inline constexpr auto cmpsw() {
        return encode<e_instruction_id::cmpsw>();
    }

    inline constexpr auto cmpsd_() {
        // Note: Using cmpsd_() to avoid conflict with SSE cmpsd
        return encode<e_instruction_id::cmpsd>();
    }

    inline constexpr auto cmpsq() {
        return encode_string_q<e_instruction_id::cmpsq>();
    }

    // SCAS - Scan String (compare al/ax/eax/rax with [rdi])
    inline constexpr auto scasb() {
        return encode<e_instruction_id::scasb>();
    }

    inline constexpr auto scasw() {
        return encode<e_instruction_id::scasw>();
    }

    inline constexpr auto scasd() {
        return encode<e_instruction_id::scasd>();
    }

    inline constexpr auto scasq() {
        return encode_string_q<e_instruction_id::scasq>();
    }

    // LODS - Load String (load from [rsi] to al/ax/eax/rax)
    inline constexpr auto lodsb() {
        return encode<e_instruction_id::lodsb>();
    }

    inline constexpr auto lodsw() {
        return encode<e_instruction_id::lodsw>();
    }

    inline constexpr auto lodsd() {
        return encode<e_instruction_id::lodsd>();
    }

    inline constexpr auto lodsq() {
        return encode_string_q<e_instruction_id::lodsq>();
    }

    // STOS - Store String (store al/ax/eax/rax to [rdi])
    inline constexpr auto stosb() {
        return encode<e_instruction_id::stosb>();
    }

    inline constexpr auto stosw() {
        return encode<e_instruction_id::stosw>();
    }

    inline constexpr auto stosd() {
        return encode<e_instruction_id::stosd>();
    }

    inline constexpr auto stosq() {
        return encode_string_q<e_instruction_id::stosq>();
    }

    // ==========================================================================
    // REP Prefix Wrappers
    // ==========================================================================

    // REP MOVS - Repeat move string while rcx > 0
    inline constexpr auto rep_movsb() {
        return with_rep_prefix(rep_prefix::rep, movsb());
    }

    inline constexpr auto rep_movsw() {
        return with_rep_prefix(rep_prefix::rep, movsw());
    }

    inline constexpr auto rep_movsd() {
        return with_rep_prefix(rep_prefix::rep, movsd_());
    }

    inline constexpr auto rep_movsq() {
        return with_rep_prefix(rep_prefix::rep, movsq());
    }

    // REP STOS - Repeat store string while rcx > 0
    inline constexpr auto rep_stosb() {
        return with_rep_prefix(rep_prefix::rep, stosb());
    }

    inline constexpr auto rep_stosw() {
        return with_rep_prefix(rep_prefix::rep, stosw());
    }

    inline constexpr auto rep_stosd() {
        return with_rep_prefix(rep_prefix::rep, stosd());
    }

    inline constexpr auto rep_stosq() {
        return with_rep_prefix(rep_prefix::rep, stosq());
    }

    // REP LODS - Repeat load string while rcx > 0
    inline constexpr auto rep_lodsb() {
        return with_rep_prefix(rep_prefix::rep, lodsb());
    }

    inline constexpr auto rep_lodsw() {
        return with_rep_prefix(rep_prefix::rep, lodsw());
    }

    inline constexpr auto rep_lodsd() {
        return with_rep_prefix(rep_prefix::rep, lodsd());
    }

    inline constexpr auto rep_lodsq() {
        return with_rep_prefix(rep_prefix::rep, lodsq());
    }

    // REPE/REPZ CMPS - Repeat compare while equal/zero and rcx > 0
    inline constexpr auto repe_cmpsb() {
        return with_rep_prefix(rep_prefix::repe, cmpsb());
    }

    inline constexpr auto repe_cmpsw() {
        return with_rep_prefix(rep_prefix::repe, cmpsw());
    }

    inline constexpr auto repe_cmpsd() {
        return with_rep_prefix(rep_prefix::repe, cmpsd_());
    }

    inline constexpr auto repe_cmpsq() {
        return with_rep_prefix(rep_prefix::repe, cmpsq());
    }

    // REPNE/REPNZ CMPS - Repeat compare while not equal/not zero and rcx > 0
    inline constexpr auto repne_cmpsb() {
        return with_rep_prefix(rep_prefix::repne, cmpsb());
    }

    inline constexpr auto repne_cmpsw() {
        return with_rep_prefix(rep_prefix::repne, cmpsw());
    }

    inline constexpr auto repne_cmpsd() {
        return with_rep_prefix(rep_prefix::repne, cmpsd_());
    }

    inline constexpr auto repne_cmpsq() {
        return with_rep_prefix(rep_prefix::repne, cmpsq());
    }

    // REPE/REPZ SCAS - Repeat scan while equal/zero and rcx > 0
    inline constexpr auto repe_scasb() {
        return with_rep_prefix(rep_prefix::repe, scasb());
    }

    inline constexpr auto repe_scasw() {
        return with_rep_prefix(rep_prefix::repe, scasw());
    }

    inline constexpr auto repe_scasd() {
        return with_rep_prefix(rep_prefix::repe, scasd());
    }

    inline constexpr auto repe_scasq() {
        return with_rep_prefix(rep_prefix::repe, scasq());
    }

    // REPNE/REPNZ SCAS - Repeat scan while not equal/not zero and rcx > 0
    inline constexpr auto repne_scasb() {
        return with_rep_prefix(rep_prefix::repne, scasb());
    }

    inline constexpr auto repne_scasw() {
        return with_rep_prefix(rep_prefix::repne, scasw());
    }

    inline constexpr auto repne_scasd() {
        return with_rep_prefix(rep_prefix::repne, scasd());
    }

    inline constexpr auto repne_scasq() {
        return with_rep_prefix(rep_prefix::repne, scasq());
    }

    // Aliases: repz = repe, repnz = repne
    inline constexpr auto repz_cmpsb() {
        return repe_cmpsb();
    }
    inline constexpr auto repz_cmpsw() {
        return repe_cmpsw();
    }
    inline constexpr auto repz_cmpsd() {
        return repe_cmpsd();
    }
    inline constexpr auto repz_cmpsq() {
        return repe_cmpsq();
    }

    inline constexpr auto repnz_cmpsb() {
        return repne_cmpsb();
    }
    inline constexpr auto repnz_cmpsw() {
        return repne_cmpsw();
    }
    inline constexpr auto repnz_cmpsd() {
        return repne_cmpsd();
    }
    inline constexpr auto repnz_cmpsq() {
        return repne_cmpsq();
    }

    inline constexpr auto repz_scasb() {
        return repe_scasb();
    }
    inline constexpr auto repz_scasw() {
        return repe_scasw();
    }
    inline constexpr auto repz_scasd() {
        return repe_scasd();
    }
    inline constexpr auto repz_scasq() {
        return repe_scasq();
    }

    inline constexpr auto repnz_scasb() {
        return repne_scasb();
    }
    inline constexpr auto repnz_scasw() {
        return repne_scasw();
    }
    inline constexpr auto repnz_scasd() {
        return repne_scasd();
    }
    inline constexpr auto repnz_scasq() {
        return repne_scasq();
    }

    // ==========================================================================
    // MOVZX - Move with Zero-Extend
    // ==========================================================================
    // MOVZX r16, r/m8:  66 0F B6 /r
    // MOVZX r32, r/m8:  0F B6 /r
    // MOVZX r64, r/m8:  REX.W 0F B6 /r
    // MOVZX r32, r/m16: 0F B7 /r
    // MOVZX r64, r/m16: REX.W 0F B7 /r

    // MOVZX with 8-bit register source
    template<typename Dest, typename Src>
        requires Register<Dest> && Register8<Src> && (Dest::size >= 16)
    inline constexpr auto movzx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movzx>(dest, src);
    }

    // MOVZX with 16-bit register source
    template<typename Dest, typename Src>
        requires Register<Dest> && Register16<Src> && (Dest::size >= 32)
    inline constexpr auto movzx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movzx>(dest, src);
    }

    // MOVZX with 8-bit memory source
    template<typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 16) && (Src::size == 8)
    inline constexpr auto movzx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movzx>(dest, src);
    }

    // MOVZX with 16-bit memory source
    template<typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 32) && (Src::size == 16)
    inline constexpr auto movzx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movzx>(dest, src);
    }

    // ==========================================================================
    // MOVSX - Move with Sign-Extend
    // ==========================================================================
    // MOVSX r16, r/m8:  66 0F BE /r
    // MOVSX r32, r/m8:  0F BE /r
    // MOVSX r64, r/m8:  REX.W 0F BE /r
    // MOVSX r32, r/m16: 0F BF /r
    // MOVSX r64, r/m16: REX.W 0F BF /r

    // MOVSX with 8-bit register source
    template<typename Dest, typename Src>
        requires Register<Dest> && Register8<Src> && (Dest::size >= 16)
    inline constexpr auto movsx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsx>(dest, src);
    }

    // MOVSX with 16-bit register source
    template<typename Dest, typename Src>
        requires Register<Dest> && Register16<Src> && (Dest::size >= 32)
    inline constexpr auto movsx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsx>(dest, src);
    }

    // MOVSX with 8-bit memory source
    template<typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 16) && (Src::size == 8)
    inline constexpr auto movsx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsx>(dest, src);
    }

    // MOVSX with 16-bit memory source
    template<typename Dest, typename Src>
        requires Register<Dest> && Memory<Src> && (Dest::size >= 32) && (Src::size == 16)
    inline constexpr auto movsx(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsx>(dest, src);
    }

    // ==========================================================================
    // MOVSXD - Move with Sign-Extend Doubleword
    // ==========================================================================
    // MOVSXD r64, r/m32: REX.W 63 /r
    // Sign-extends a 32-bit value to 64-bit

    // MOVSXD with 32-bit register source
    template<typename Dest, typename Src>
        requires Register64<Dest> && Register32<Src>
    inline constexpr auto movsxd(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsxd>(dest, src);
    }

    // MOVSXD with 32-bit memory source
    template<typename Dest, typename Src>
        requires Register64<Dest> && Memory<Src> && (Src::size == 32)
    inline constexpr auto movsxd(const Dest& dest, const Src& src) {
        return encode<e_instruction_id::movsxd>(dest, src);
    }

    // ==========================================================================
    // System Instructions
    // ==========================================================================

    // SYSCALL - Fast System Call (64-bit mode)
    // Opcode: 0F 05
    inline constexpr auto syscall_() {
        return encode<e_instruction_id::syscall_>();
    }

    // SYSENTER - Fast System Call (32-bit mode, also works in 64-bit)
    // Opcode: 0F 34
    inline constexpr auto sysenter() {
        return encode<e_instruction_id::sysenter>();
    }

    // SYSEXIT - Fast Return from System Call
    // Opcode: 0F 35
    inline constexpr auto sysexit() {
        return encode<e_instruction_id::sysexit>();
    }

    // INT3 - Breakpoint (Debug Trap)
    // Opcode: CC
    inline constexpr auto int3() {
        return encode<e_instruction_id::int3>();
    }

    // INT imm8 - Software Interrupt
    // Opcode: CD ib
    inline constexpr auto int_(std::uint8_t vector) {
        return encode<e_instruction_id::int_>(vector);
    }

    // Convenience aliases for common interrupts
    inline constexpr auto int_0x80() {
        return int_(0x80); // Linux 32-bit syscall
    }

    inline constexpr auto int_0x2e() {
        return int_(0x2E); // Windows syscall (legacy)
    }

    // INTO - Interrupt on Overflow (Invalid in 64-bit mode)
    // Opcode: CE
    inline constexpr auto into() {
        return encode<e_instruction_id::into>();
    }

    // IRET - Interrupt Return (16-bit operand size)
    // Opcode: CF
    inline constexpr auto iret() {
        return encode<e_instruction_id::iret>();
    }

    // IRETD - Interrupt Return (32-bit operand size)
    // Opcode: CF (same as IRET, operand size determines behavior)
    inline constexpr auto iretd() {
        return encode<e_instruction_id::iretd>();
    }

    // IRETQ - Interrupt Return (64-bit operand size)
    // Opcode: 48 CF (REX.W + CF)
    inline constexpr auto iretq() {
        return encode_iretq();
    }

    // CLI - Clear Interrupt Flag
    // Opcode: FA
    inline constexpr auto cli() {
        return encode<e_instruction_id::cli>();
    }

    // STI - Set Interrupt Flag
    // Opcode: FB
    inline constexpr auto sti() {
        return encode<e_instruction_id::sti>();
    }

    // HLT - Halt
    // Opcode: F4
    inline constexpr auto hlt() {
        return encode<e_instruction_id::hlt>();
    }

    // CPUID - CPU Identification
    // Opcode: 0F A2
    inline constexpr auto cpuid() {
        return encode<e_instruction_id::cpuid>();
    }

    // RDTSC - Read Time-Stamp Counter
    // Opcode: 0F 31
    inline constexpr auto rdtsc() {
        return encode<e_instruction_id::rdtsc>();
    }

    // RDTSCP - Read Time-Stamp Counter and Processor ID
    // Opcode: 0F 01 F9
    inline constexpr auto rdtscp() {
        return encode<e_instruction_id::rdtscp>();
    }

    // ==========================================================================
    // LOCK Prefix Wrapper
    // ==========================================================================
    //
    // Prepends the LOCK prefix (0xF0) to a read-modify-write memory instruction,
    // making it atomic. Wraps an already-encoded instruction, so it composes with
    // any memory-form ALU/bit instruction:
    //
    //     lock_(inc(qword_ptr(rcx + 0x20)))   // F0 48 FF 41 20
    //     lock_(add(dword_ptr(rax), ecx))     // F0 01 08
    //     lock_(xchg(qword_ptr(rbx), rax))    // F0 48 87 03
    //
    // LOCK is only valid on memory destinations for a fixed instruction set
    // (ADD, ADC, AND, BTC, BTR, BTS, CMPXCHG, CMPXCHG8B/16B, DEC, INC, NEG, NOT,
    // OR, SBB, SUB, XOR, XADD, XCHG). This wrapper does not enforce that; it is a
    // thin byte-level prefix, matching how with_rep_prefix exposes REP.
    template<std::size_t N>
    inline constexpr auto lock_(const std::array<std::uint8_t, N>& instr) {
        return with_lock_prefix(instr);
    }

} // namespace static_asm::x86::instructions
