#include "static_asm.hpp"
#include <gtest/gtest.h>

using namespace static_asm;
using namespace static_asm::x86;
using namespace static_asm::x86::registers;
using namespace static_asm::x86::instructions;

// ==========================================================================
// MOVS - Move String
// ==========================================================================

TEST(StringInstructions, MovsBasic) {
    // MOVSB: A4
    EXPECT_EQ(movsb(), (internal::make_array<std::uint8_t>(0xA4)));

    // MOVSW: 66 A5
    EXPECT_EQ(movsw(), (internal::make_array<std::uint8_t>(0x66, 0xA5)));

    // MOVSD: A5
    EXPECT_EQ(movsd_(), (internal::make_array<std::uint8_t>(0xA5)));

    // MOVSQ: 48 A5 (REX.W prefix)
    EXPECT_EQ(movsq(), (internal::make_array<std::uint8_t>(0x48, 0xA5)));
}

TEST(StringInstructions, RepMovs) {
    // REP MOVSB: F3 A4
    EXPECT_EQ(rep_movsb(), (internal::make_array<std::uint8_t>(0xF3, 0xA4)));

    // REP MOVSW: F3 66 A5
    EXPECT_EQ(rep_movsw(), (internal::make_array<std::uint8_t>(0xF3, 0x66, 0xA5)));

    // REP MOVSD: F3 A5
    EXPECT_EQ(rep_movsd(), (internal::make_array<std::uint8_t>(0xF3, 0xA5)));

    // REP MOVSQ: F3 48 A5
    EXPECT_EQ(rep_movsq(), (internal::make_array<std::uint8_t>(0xF3, 0x48, 0xA5)));
}

// ==========================================================================
// CMPS - Compare String
// ==========================================================================

TEST(StringInstructions, CmpsBasic) {
    // CMPSB: A6
    EXPECT_EQ(cmpsb(), (internal::make_array<std::uint8_t>(0xA6)));

    // CMPSW: 66 A7
    EXPECT_EQ(cmpsw(), (internal::make_array<std::uint8_t>(0x66, 0xA7)));

    // CMPSD: A7
    EXPECT_EQ(cmpsd_(), (internal::make_array<std::uint8_t>(0xA7)));

    // CMPSQ: 48 A7 (REX.W prefix)
    EXPECT_EQ(cmpsq(), (internal::make_array<std::uint8_t>(0x48, 0xA7)));
}

TEST(StringInstructions, RepeCmps) {
    // REPE CMPSB: F3 A6
    EXPECT_EQ(repe_cmpsb(), (internal::make_array<std::uint8_t>(0xF3, 0xA6)));

    // REPE CMPSW: F3 66 A7
    EXPECT_EQ(repe_cmpsw(), (internal::make_array<std::uint8_t>(0xF3, 0x66, 0xA7)));

    // REPE CMPSD: F3 A7
    EXPECT_EQ(repe_cmpsd(), (internal::make_array<std::uint8_t>(0xF3, 0xA7)));

    // REPE CMPSQ: F3 48 A7
    EXPECT_EQ(repe_cmpsq(), (internal::make_array<std::uint8_t>(0xF3, 0x48, 0xA7)));
}

TEST(StringInstructions, RepneCmps) {
    // REPNE CMPSB: F2 A6
    EXPECT_EQ(repne_cmpsb(), (internal::make_array<std::uint8_t>(0xF2, 0xA6)));

    // REPNE CMPSW: F2 66 A7
    EXPECT_EQ(repne_cmpsw(), (internal::make_array<std::uint8_t>(0xF2, 0x66, 0xA7)));

    // REPNE CMPSD: F2 A7
    EXPECT_EQ(repne_cmpsd(), (internal::make_array<std::uint8_t>(0xF2, 0xA7)));

    // REPNE CMPSQ: F2 48 A7
    EXPECT_EQ(repne_cmpsq(), (internal::make_array<std::uint8_t>(0xF2, 0x48, 0xA7)));
}

TEST(StringInstructions, RepzCmpsAliases) {
    // REPZ should be alias for REPE
    EXPECT_EQ(repz_cmpsb(), repe_cmpsb());
    EXPECT_EQ(repz_cmpsw(), repe_cmpsw());
    EXPECT_EQ(repz_cmpsd(), repe_cmpsd());
    EXPECT_EQ(repz_cmpsq(), repe_cmpsq());

    // REPNZ should be alias for REPNE
    EXPECT_EQ(repnz_cmpsb(), repne_cmpsb());
    EXPECT_EQ(repnz_cmpsw(), repne_cmpsw());
    EXPECT_EQ(repnz_cmpsd(), repne_cmpsd());
    EXPECT_EQ(repnz_cmpsq(), repne_cmpsq());
}

// ==========================================================================
// SCAS - Scan String
// ==========================================================================

TEST(StringInstructions, ScasBasic) {
    // SCASB: AE
    EXPECT_EQ(scasb(), (internal::make_array<std::uint8_t>(0xAE)));

    // SCASW: 66 AF
    EXPECT_EQ(scasw(), (internal::make_array<std::uint8_t>(0x66, 0xAF)));

    // SCASD: AF
    EXPECT_EQ(scasd(), (internal::make_array<std::uint8_t>(0xAF)));

    // SCASQ: 48 AF (REX.W prefix)
    EXPECT_EQ(scasq(), (internal::make_array<std::uint8_t>(0x48, 0xAF)));
}

TEST(StringInstructions, RepeScas) {
    // REPE SCASB: F3 AE
    EXPECT_EQ(repe_scasb(), (internal::make_array<std::uint8_t>(0xF3, 0xAE)));

    // REPE SCASW: F3 66 AF
    EXPECT_EQ(repe_scasw(), (internal::make_array<std::uint8_t>(0xF3, 0x66, 0xAF)));

    // REPE SCASD: F3 AF
    EXPECT_EQ(repe_scasd(), (internal::make_array<std::uint8_t>(0xF3, 0xAF)));

    // REPE SCASQ: F3 48 AF
    EXPECT_EQ(repe_scasq(), (internal::make_array<std::uint8_t>(0xF3, 0x48, 0xAF)));
}

TEST(StringInstructions, RepneScas) {
    // REPNE SCASB: F2 AE
    EXPECT_EQ(repne_scasb(), (internal::make_array<std::uint8_t>(0xF2, 0xAE)));

    // REPNE SCASW: F2 66 AF
    EXPECT_EQ(repne_scasw(), (internal::make_array<std::uint8_t>(0xF2, 0x66, 0xAF)));

    // REPNE SCASD: F2 AF
    EXPECT_EQ(repne_scasd(), (internal::make_array<std::uint8_t>(0xF2, 0xAF)));

    // REPNE SCASQ: F2 48 AF
    EXPECT_EQ(repne_scasq(), (internal::make_array<std::uint8_t>(0xF2, 0x48, 0xAF)));
}

TEST(StringInstructions, RepzScasAliases) {
    // REPZ should be alias for REPE
    EXPECT_EQ(repz_scasb(), repe_scasb());
    EXPECT_EQ(repz_scasw(), repe_scasw());
    EXPECT_EQ(repz_scasd(), repe_scasd());
    EXPECT_EQ(repz_scasq(), repe_scasq());

    // REPNZ should be alias for REPNE
    EXPECT_EQ(repnz_scasb(), repne_scasb());
    EXPECT_EQ(repnz_scasw(), repne_scasw());
    EXPECT_EQ(repnz_scasd(), repne_scasd());
    EXPECT_EQ(repnz_scasq(), repne_scasq());
}

// ==========================================================================
// LODS - Load String
// ==========================================================================

TEST(StringInstructions, LodsBasic) {
    // LODSB: AC
    EXPECT_EQ(lodsb(), (internal::make_array<std::uint8_t>(0xAC)));

    // LODSW: 66 AD
    EXPECT_EQ(lodsw(), (internal::make_array<std::uint8_t>(0x66, 0xAD)));

    // LODSD: AD
    EXPECT_EQ(lodsd(), (internal::make_array<std::uint8_t>(0xAD)));

    // LODSQ: 48 AD (REX.W prefix)
    EXPECT_EQ(lodsq(), (internal::make_array<std::uint8_t>(0x48, 0xAD)));
}

TEST(StringInstructions, RepLods) {
    // REP LODSB: F3 AC
    EXPECT_EQ(rep_lodsb(), (internal::make_array<std::uint8_t>(0xF3, 0xAC)));

    // REP LODSW: F3 66 AD
    EXPECT_EQ(rep_lodsw(), (internal::make_array<std::uint8_t>(0xF3, 0x66, 0xAD)));

    // REP LODSD: F3 AD
    EXPECT_EQ(rep_lodsd(), (internal::make_array<std::uint8_t>(0xF3, 0xAD)));

    // REP LODSQ: F3 48 AD
    EXPECT_EQ(rep_lodsq(), (internal::make_array<std::uint8_t>(0xF3, 0x48, 0xAD)));
}

// ==========================================================================
// STOS - Store String
// ==========================================================================

TEST(StringInstructions, StosBasic) {
    // STOSB: AA
    EXPECT_EQ(stosb(), (internal::make_array<std::uint8_t>(0xAA)));

    // STOSW: 66 AB
    EXPECT_EQ(stosw(), (internal::make_array<std::uint8_t>(0x66, 0xAB)));

    // STOSD: AB
    EXPECT_EQ(stosd(), (internal::make_array<std::uint8_t>(0xAB)));

    // STOSQ: 48 AB (REX.W prefix)
    EXPECT_EQ(stosq(), (internal::make_array<std::uint8_t>(0x48, 0xAB)));
}

TEST(StringInstructions, RepStos) {
    // REP STOSB: F3 AA
    EXPECT_EQ(rep_stosb(), (internal::make_array<std::uint8_t>(0xF3, 0xAA)));

    // REP STOSW: F3 66 AB
    EXPECT_EQ(rep_stosw(), (internal::make_array<std::uint8_t>(0xF3, 0x66, 0xAB)));

    // REP STOSD: F3 AB
    EXPECT_EQ(rep_stosd(), (internal::make_array<std::uint8_t>(0xF3, 0xAB)));

    // REP STOSQ: F3 48 AB
    EXPECT_EQ(rep_stosq(), (internal::make_array<std::uint8_t>(0xF3, 0x48, 0xAB)));
}

// ==========================================================================
// Integration Tests - Assembling multiple string instructions
// ==========================================================================

TEST(StringInstructions, AssembleStringOperations) {
    // Test assembling a simple memcpy-like sequence
    constexpr auto memcpy_like = core::assemble(
        rep_movsb());
    EXPECT_EQ(memcpy_like, (internal::make_array<std::uint8_t>(0xF3, 0xA4)));

    // Test assembling a strlen-like sequence (REPNE SCASB)
    constexpr auto strlen_like = core::assemble(
        repne_scasb());
    EXPECT_EQ(strlen_like, (internal::make_array<std::uint8_t>(0xF2, 0xAE)));

    // Test assembling a memset-like sequence
    constexpr auto memset_like = core::assemble(
        rep_stosb());
    EXPECT_EQ(memset_like, (internal::make_array<std::uint8_t>(0xF3, 0xAA)));

    // Test assembling memcmp-like sequence
    constexpr auto memcmp_like = core::assemble(
        repe_cmpsb());
    EXPECT_EQ(memcmp_like, (internal::make_array<std::uint8_t>(0xF3, 0xA6)));
}

TEST(StringInstructions, AssembleComplexSequence) {
    // A more complex sequence combining string instructions with regular instructions
    constexpr auto code = core::assemble(
        xor_(rax, rax), // Clear rax
        stosq(), // Store qword
        movsb(), // Move byte
        lodsb(), // Load byte
        cmpsb() // Compare byte
    );

    // Expected: 48 31 C0 (xor rax, rax) + 48 AB (stosq) + A4 (movsb) + AC (lodsb) + A6 (cmpsb)
    EXPECT_EQ(code, (internal::make_array<std::uint8_t>(
                        0x48, 0x31, 0xC0, // xor rax, rax
                        0x48, 0xAB, // stosq
                        0xA4, // movsb
                        0xAC, // lodsb
                        0xA6 // cmpsb
                        )));
}
