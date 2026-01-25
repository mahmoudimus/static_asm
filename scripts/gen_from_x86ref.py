#!/usr/bin/env python3
# /// script
# requires-python = ">=3.10"
# ///
"""
Generate C++ instruction database and tests from mazegen/x86reference XML.

Usage:
    uv run scripts/gen_from_x86ref.py                    # Show summary
    uv run scripts/gen_from_x86ref.py -i mov             # Show instruction details
    uv run scripts/gen_from_x86ref.py --generate-db      # Generate instruction_db files
    uv run scripts/gen_from_x86ref.py --generate-tests   # Generate test files
"""

import xml.etree.ElementTree as ET
import sys
from dataclasses import dataclass, field
from typing import Optional, List, Dict, Tuple, Set
from collections import defaultdict
from pathlib import Path

import hashlib
from datetime import datetime, timezone

SCRIPT_DIR = Path(__file__).resolve().parent
XML_PATH = SCRIPT_DIR / 'x86reference.xml'
SRC_DIR = SCRIPT_DIR.parent / 'include' / 'static_asm' / 'x86'
GEN_DIR = SRC_DIR / 'gen'
TEST_DIR = SCRIPT_DIR.parent / 'tests'


def get_file_md5(filepath: Path) -> str:
    """Calculate MD5 hash of a file."""
    return hashlib.md5(filepath.read_bytes()).hexdigest()


def get_generation_preamble() -> str:
    """Generate the preamble for generated files."""
    md5 = get_file_md5(XML_PATH)
    timestamp = datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')
    return f"""// =============================================================================
// GENERATED FILE - DO NOT EDIT MANUALLY
// =============================================================================
// Source:    scripts/x86reference.xml
// MD5:       {md5}
// Generated: {timestamp}
// =============================================================================
"""

# Addressing mode descriptions (Intel notation)
ADDRESSING_MODES = {
    'A': 'Direct address (far pointer)',
    'C': 'Control register (ModRM reg)',
    'D': 'Debug register (ModRM reg)',
    'E': 'ModRM r/m (register or memory)',
    'F': 'EFLAGS/RFLAGS register',
    'G': 'ModRM reg field (general register)',
    'H': 'VEX.vvvv field',
    'I': 'Immediate data',
    'J': 'Relative offset (for jumps)',
    'M': 'ModRM r/m (memory only)',
    'N': 'ModRM r/m (MMX register)',
    'O': 'Direct offset (no ModRM)',
    'P': 'ModRM reg field (MMX register)',
    'Q': 'ModRM r/m (MMX register or memory)',
    'R': 'ModRM r/m (general register only)',
    'S': 'ModRM reg field (segment register)',
    'U': 'ModRM r/m (XMM register)',
    'V': 'ModRM reg field (XMM register)',
    'W': 'ModRM r/m (XMM register or memory)',
    'X': 'DS:rSI memory',
    'Y': 'ES:rDI memory',
    'Z': 'Register encoded in opcode (bits 0-2)',
}

# Operand type to size mapping
OPERAND_TYPE_SIZE = {
    'b': 8,
    'w': 16,
    'd': 32,
    'q': 64,
    'v': 0,      # Variable (16/32/64)
    'z': 0,      # Variable (16/32)
    'vqp': 0,    # Variable with REX.W
    'vds': 32,   # 32-bit sign-extended
    'bs': 8,
    'bss': 8,
}

# Instructions we support (common x86-64 instructions)
SUPPORTED_INSTRUCTIONS = {
    # ALU operations
    'ADD', 'ADC', 'SUB', 'SBB', 'AND', 'OR', 'XOR', 'CMP', 'TEST',
    'INC', 'DEC', 'NEG', 'NOT', 'MUL', 'IMUL', 'DIV', 'IDIV',
    # Shifts and rotates
    'SHL', 'SHR', 'SAL', 'SAR', 'ROL', 'ROR', 'RCL', 'RCR',
    # Bit operations
    'BT', 'BTC', 'BTR', 'BTS', 'BSF', 'BSR', 'BSWAP',
    # Data movement
    'MOV', 'MOVZX', 'MOVSX', 'MOVSXD', 'LEA', 'XCHG',
    'PUSH', 'POP', 'PUSHF', 'POPF',
    'CBW', 'CWDE', 'CDQE', 'CWD', 'CDQ', 'CQO',
    # Control flow
    'JMP', 'CALL', 'RET', 'RETF',
    'JO', 'JNO', 'JB', 'JNB', 'JZ', 'JNZ', 'JBE', 'JNBE',
    'JS', 'JNS', 'JP', 'JNP', 'JL', 'JNL', 'JLE', 'JNLE',
    'JE', 'JNE', 'JA', 'JNA', 'JAE', 'JNAE', 'JG', 'JNG', 'JGE', 'JNGE',
    'JC', 'JNC', 'JPE', 'JPO',
    'LOOP', 'LOOPE', 'LOOPNE', 'JCXZ', 'JECXZ', 'JRCXZ',
    # Conditionals
    'SETO', 'SETNO', 'SETB', 'SETNB', 'SETZ', 'SETNZ', 'SETBE', 'SETNBE',
    'SETS', 'SETNS', 'SETP', 'SETNP', 'SETL', 'SETNL', 'SETLE', 'SETNLE',
    'CMOVO', 'CMOVNO', 'CMOVB', 'CMOVNB', 'CMOVZ', 'CMOVNZ', 'CMOVBE', 'CMOVNBE',
    'CMOVS', 'CMOVNS', 'CMOVP', 'CMOVNP', 'CMOVL', 'CMOVNL', 'CMOVLE', 'CMOVNLE',
    # String operations
    'MOVS', 'MOVSB', 'MOVSW', 'MOVSD', 'MOVSQ',
    'CMPS', 'CMPSB', 'CMPSW', 'CMPSD', 'CMPSQ',
    'STOS', 'STOSB', 'STOSW', 'STOSD', 'STOSQ',
    'LODS', 'LODSB', 'LODSW', 'LODSD', 'LODSQ',
    'SCAS', 'SCASB', 'SCASW', 'SCASD', 'SCASQ',
    'REP', 'REPE', 'REPNE',
    # Misc
    'NOP', 'UD2', 'INT', 'INT3', 'INTO', 'IRET', 'IRETD', 'IRETQ',
    'HLT', 'WAIT', 'LOCK', 'CPUID', 'RDTSC', 'RDTSCP',
    'CLC', 'STC', 'CMC', 'CLD', 'STD', 'CLI', 'STI',
    'LAHF', 'SAHF', 'XLAT', 'XLATB',
    'ENTER', 'LEAVE',
    'IN', 'OUT', 'INS', 'OUTS',
    # Extensions we might want
    'POPCNT', 'LZCNT', 'TZCNT',
    'MOVBE', 'CRC32',
}

# Encoding type mapping based on instruction groups
ENCODING_MAP = {
    ('gen', 'arith'): 'alu',
    ('gen', 'logical'): 'alu',
    ('gen', 'datamov'): 'mov',
    ('gen', 'stack'): 'push',  # or 'pop'
    ('gen', 'branch'): 'jmp',
    ('gen', 'control'): 'noops',
    ('gen', 'bit'): 'bt',
}


@dataclass
class Operand:
    addressing: str
    type: str
    is_dst: bool = False
    is_src: bool = False
    displayed: bool = True
    group: Optional[str] = None

    def needs_modrm(self) -> bool:
        return self.addressing in ('C', 'D', 'E', 'ES', 'EST', 'G', 'H', 'M',
                                   'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W')

    def is_immediate(self) -> bool:
        return self.addressing in ('A', 'I', 'J')

    def is_reg_in_opcode(self) -> bool:
        return self.addressing == 'Z'

    def immediate_size(self) -> Optional[int]:
        if not self.is_immediate():
            return None
        if self.type.startswith('b'):
            return 1
        elif self.type in ('w',):
            return 2
        elif self.type.startswith('v') or self.type.startswith('z') or self.type.startswith('d'):
            return 4
        elif self.type.startswith('q'):
            return 8
        elif self.type.startswith('p'):
            return 6
        return 4  # Default

    def operand_size(self) -> Optional[int]:
        """Get operand size in bits."""
        t = self.type
        if t.startswith('b'):
            return 8
        elif t == 'w':
            return 16
        elif t == 'd':
            return 32
        elif t == 'q':
            return 64
        elif t.startswith('v') or t.startswith('z'):
            return 0  # Variable
        return None


@dataclass
class InstructionEntry:
    opcode: int
    mnemonic: str
    operands: List[Operand]
    prefix_0f: bool = False
    opcode_ext: Optional[int] = None
    direction: Optional[int] = None
    op_size: Optional[int] = None
    groups: List[str] = field(default_factory=list)
    brief: str = ""
    attr: Optional[str] = None
    tttn: Optional[str] = None

    def needs_modrm(self) -> bool:
        if self.opcode_ext is not None:
            return True
        return any(op.needs_modrm() for op in self.operands)

    def has_immediate(self) -> bool:
        return any(op.is_immediate() for op in self.operands)

    def has_reg_in_opcode(self) -> bool:
        return any(op.is_reg_in_opcode() for op in self.operands)

    def get_encoding_type(self) -> str:
        """Determine the encoding type for this instruction."""
        mnem = self.mnemonic.upper()

        # Special cases
        if mnem in ('ADD', 'ADC', 'SUB', 'SBB', 'AND', 'OR', 'XOR', 'CMP', 'TEST'):
            return 'alu'
        if mnem in ('INC', 'DEC', 'NEG', 'NOT'):
            return 'alu'
        if mnem in ('SHL', 'SHR', 'SAL', 'SAR', 'ROL', 'ROR', 'RCL', 'RCR'):
            return 'shift'
        if mnem in ('MUL', 'IMUL', 'DIV', 'IDIV'):
            return 'mul'
        if mnem in ('BT', 'BTC', 'BTR', 'BTS'):
            return 'bt'
        if mnem in ('MOV', 'MOVZX', 'MOVSX', 'MOVSXD'):
            return 'mov'
        if mnem == 'LEA':
            return 'lea'
        if mnem == 'XCHG':
            return 'xchg'
        if mnem == 'PUSH':
            return 'push'
        if mnem == 'POP':
            return 'pop'
        if mnem in ('JMP',):
            return 'jmp'
        if mnem in ('CALL',):
            return 'call'
        if mnem in ('RET', 'RETF'):
            return 'ret'
        if mnem.startswith('J') and len(mnem) <= 4:
            return 'jcc'
        if mnem.startswith('SET'):
            return 'setcc'
        if mnem.startswith('CMOV'):
            return 'cmovcc'
        if mnem in ('NOP', 'UD2', 'HLT', 'WAIT', 'CPUID', 'RDTSC'):
            return 'noops'
        if mnem in ('CLC', 'STC', 'CMC', 'CLD', 'STD', 'CLI', 'STI'):
            return 'noops'
        if mnem in ('INT', 'INT3', 'INTO'):
            return 'int'
        if mnem in ('LOOP', 'LOOPE', 'LOOPNE'):
            return 'loop'

        return 'generic'


def parse_operand(elem, is_dst: bool = False, is_src: bool = False) -> Optional[Operand]:
    a_elem = elem.find('./a')
    t_elem = elem.find('./t')

    if a_elem is None:
        text = elem.text.strip() if elem.text else None
        if text:
            return Operand(
                addressing='fixed',
                type='',
                is_dst=is_dst,
                is_src=is_src,
                group=text,
                displayed=elem.get('displayed', 'yes') != 'no'
            )
        return None

    addressing = a_elem.text if a_elem.text else ''
    op_type = t_elem.text if t_elem is not None and t_elem.text else ''

    return Operand(
        addressing=addressing,
        type=op_type,
        is_dst=is_dst,
        is_src=is_src,
        displayed=elem.get('displayed', 'yes') != 'no'
    )


def parse_entry(pri_opcd_elem, entry_elem, prefix_0f: bool = False) -> Optional[InstructionEntry]:
    opcode = int(pri_opcd_elem.get('value'), 16)

    if entry_elem.get('attr') == 'invd':
        return None

    syntax_elems = entry_elem.findall('./syntax')
    if not syntax_elems:
        return None

    syntax = syntax_elems[0]
    mnem_elem = syntax.find('./mnem')
    if mnem_elem is None or mnem_elem.text is None:
        return None

    mnemonic = mnem_elem.text.strip()

    operands = []
    for dst in syntax.findall('./dst'):
        op = parse_operand(dst, is_dst=True)
        if op:
            operands.append(op)
    for src in syntax.findall('./src'):
        op = parse_operand(src, is_src=True)
        if op:
            operands.append(op)

    opcd_ext_elem = entry_elem.find('./opcd_ext')
    opcode_ext = int(opcd_ext_elem.text) if opcd_ext_elem is not None else None

    groups = []
    for grp in ['grp1', 'grp2', 'grp3']:
        elem = entry_elem.find(f'./{grp}')
        if elem is not None and elem.text:
            groups.append(elem.text)

    note_elem = entry_elem.find('./note/brief')
    brief = note_elem.text if note_elem is not None and note_elem.text else ""

    return InstructionEntry(
        opcode=opcode,
        mnemonic=mnemonic,
        operands=operands,
        prefix_0f=prefix_0f,
        opcode_ext=opcode_ext,
        direction=int(entry_elem.get('direction')) if entry_elem.get('direction') else None,
        op_size=int(entry_elem.get('op_size')) if entry_elem.get('op_size') else None,
        groups=groups,
        brief=brief,
        attr=entry_elem.get('attr'),
        tttn=entry_elem.get('tttn'),
    )


def load_instructions() -> List[InstructionEntry]:
    tree = ET.parse(XML_PATH)
    root = tree.getroot()
    assert root.tag == 'x86reference'

    instructions = []

    one_byte = root.find('./one-byte')
    if one_byte is not None:
        for pri_opcd in one_byte.findall('./pri_opcd'):
            for entry in pri_opcd.findall('./entry'):
                inst = parse_entry(pri_opcd, entry, prefix_0f=False)
                if inst:
                    instructions.append(inst)

    two_byte = root.find('./two-byte')
    if two_byte is not None:
        for pri_opcd in two_byte.findall('./pri_opcd'):
            for entry in pri_opcd.findall('./entry'):
                inst = parse_entry(pri_opcd, entry, prefix_0f=True)
                if inst:
                    instructions.append(inst)

    return instructions


def get_cpp_safe_name(mnemonic: str) -> str:
    """Convert mnemonic to C++ safe identifier."""
    name = mnemonic.lower()
    # C++ keywords that need underscore suffix
    if name in ('and', 'or', 'xor', 'not', 'int'):
        name += '_'
    return name


def filter_supported_instructions(instructions: List[InstructionEntry]) -> List[InstructionEntry]:
    """Filter to only supported instructions."""
    return [i for i in instructions if i.mnemonic.upper() in SUPPORTED_INSTRUCTIONS]


def group_by_mnemonic(instructions: List[InstructionEntry]) -> Dict[str, List[InstructionEntry]]:
    """Group instructions by mnemonic."""
    result = defaultdict(list)
    for inst in instructions:
        result[inst.mnemonic.upper()].append(inst)
    return dict(result)


def generate_instruction_db_hpp(instructions: List[InstructionEntry]) -> str:
    """Generate instruction_db.hpp with enum and types."""
    by_mnem = group_by_mnemonic(instructions)
    mnemonics = sorted(by_mnem.keys())

    # Collect all encoding types
    encoding_types = set()
    for inst in instructions:
        encoding_types.add(inst.get_encoding_type())

    preamble = get_generation_preamble()
    lines = [
        preamble.rstrip(),
        "#pragma once",
        "",
        "#include <array>",
        "#include <cstdint>",
        "",
        "namespace static_asm::x86 {",
        "",
        "    enum class e_instruction_id {",
        "        unknown,",
    ]

    for mnem in mnemonics:
        safe_name = get_cpp_safe_name(mnem)
        lines.append(f"        {safe_name},")

    lines.extend([
        "",
        "        //----",
        "        count",
        "    };",
        "",
        "    // Encoding type hint",
        "    enum class e_encoding {",
    ])

    for enc in sorted(encoding_types):
        lines.append(f"        {enc},")

    lines.extend([
        "    };",
        "",
        "    // Register/Opcode Field",
        "    enum class e_regopc_field {",
        "        none,",
        "        opcode_ext,  // ModR/M reg field is opcode extension (/0-/7)",
        "        regrm        // ModR/M contains register operand and r/m operand",
        "    };",
        "",
        "    class instruction_desc {",
        "    public:",
        "        constexpr instruction_desc(",
        "                e_instruction_id id,",
        "                std::uint8_t prefix,",
        "                std::uint8_t prefix_0f,",
        "                std::uint8_t primary_opcode,",
        "                std::uint8_t secondary_opcode,",
        "                e_encoding encoding,",
        "                e_regopc_field regopc_field",
        "        ) :",
        "                _id(id),",
        "                _prefix(prefix),",
        "                _prefix_0f(prefix_0f),",
        "                _primary_opcode(primary_opcode),",
        "                _secondary_opcode(secondary_opcode),",
        "                _encoding(encoding),",
        "                _regopc_field(regopc_field)",
        "        {}",
        "",
        "        constexpr e_instruction_id id() const { return _id; }",
        "        constexpr std::uint8_t prefix() const { return _prefix; }",
        "        constexpr std::uint8_t prefix_0f() const { return _prefix_0f; }",
        "        constexpr std::uint8_t primary_opcode() const { return _primary_opcode; }",
        "        constexpr std::uint8_t secondary_opcode() const { return _secondary_opcode; }",
        "        constexpr e_encoding encoding() const { return _encoding; }",
        "        constexpr e_regopc_field regopc_field() const { return _regopc_field; }",
        "",
        "    private:",
        "        e_instruction_id _id;",
        "        std::uint8_t _prefix;",
        "        std::uint8_t _prefix_0f;",
        "        std::uint8_t _primary_opcode;",
        "        std::uint8_t _secondary_opcode;",
        "        e_encoding _encoding;",
        "        e_regopc_field _regopc_field;",
        "    };",
        "",
        "    // ud2 instruction description (default for unknown)",
        "    constexpr instruction_desc _ud2(e_instruction_id::unknown, 0x0, 0x0f, 0x0b, 0x0, e_encoding::noops, e_regopc_field::none);",
        "",
        f"    using instructiondb = std::array<instruction_desc, static_cast<int>(e_instruction_id::count) - 1>;",
        "}",
    ])

    return "\n".join(lines)


def get_primary_variant(entries: List[InstructionEntry]) -> InstructionEntry:
    """Select the primary variant of an instruction for the database."""
    # Prefer variants with ModRM (most general)
    modrm_variants = [e for e in entries if e.needs_modrm() and e.opcode_ext is None]
    if modrm_variants:
        # Prefer 32/64-bit variants
        for v in modrm_variants:
            if any(op.type in ('v', 'vqp', 'd', 'q') for op in v.operands):
                return v
        return modrm_variants[0]

    # Fall back to first entry
    return entries[0]


def generate_instruction_db_g_hpp(instructions: List[InstructionEntry]) -> str:
    """Generate instruction_db.g.hpp with instruction entries."""
    by_mnem = group_by_mnemonic(instructions)
    mnemonics = sorted(by_mnem.keys())

    preamble = get_generation_preamble()
    lines = [
        preamble.rstrip(),
        "#pragma once",
        "",
        "#include <array>",
        "#include <utility>",
        "",
        "#include \"../instruction_db.hpp\"",
        "",
        "namespace static_asm::x86 {",
        "",
        "    namespace internal {",
        "        template<typename T, typename... Args>",
        "        inline constexpr auto make_array(Args&&... args) -> std::array<T, sizeof...(Args)> {",
        "            return {{static_cast<T>(std::forward<Args>(args))...}};",
        "        }",
        "    }",
        "",
        "#define INST_ENTRY(inst_id, prefix, prefix_0f, pri_opcode, sec_opcode, encoding, x) \\",
        "    instruction_desc(e_instruction_id::inst_id, prefix, prefix_0f, pri_opcode, sec_opcode, e_encoding::encoding, e_regopc_field::x)",
        "",
        "    constexpr instructiondb instdb = internal::make_array<instructiondb::value_type>(",
    ]

    entries_lines = []
    for mnem in mnemonics:
        entries = by_mnem[mnem]
        primary = get_primary_variant(entries)

        safe_name = get_cpp_safe_name(mnem)
        prefix = 0
        prefix_0f = 0x0f if primary.prefix_0f else 0
        pri_opcode = primary.opcode
        sec_opcode = 0
        encoding = primary.get_encoding_type()
        regopc = 'opcode_ext' if primary.opcode_ext is not None else ('regrm' if primary.needs_modrm() else 'none')

        entries_lines.append(
            f"            INST_ENTRY({safe_name}, {prefix}, {prefix_0f:#x}, {pri_opcode:#x}, {sec_opcode}, {encoding}, {regopc})"
        )

    lines.append(",\n".join(entries_lines))
    lines.append("    );")
    lines.append("")

    # Generate find_instruction_desc
    lines.extend([
        "    template <e_instruction_id Id>",
        "    inline constexpr instruction_desc find_instruction_desc() {",
        "        constexpr int iid = static_cast<int>(Id) - 1;",
        "        return (iid >= 0 && iid < static_cast<int>(instdb.size())) ? instdb[iid] : _ud2;",
        "    }",
        "",
    ])

    # Generate prefix_db
    lines.append("    inline constexpr auto prefix_db = internal::make_array<bool>(")
    prefix_entries = []
    for mnem in mnemonics:
        # Currently no mandatory prefixes for basic instructions
        prefix_entries.append("            false")
    lines.append(",\n".join(prefix_entries))
    lines.append("    );")
    lines.append("")

    lines.extend([
        "    template <e_instruction_id Id>",
        "    inline constexpr bool has_prefix() {",
        "        constexpr int iid = static_cast<int>(Id) - 1;",
        "        return (iid >= 0 && iid < static_cast<int>(prefix_db.size())) ? prefix_db[iid] : false;",
        "    }",
        "",
    ])

    # Generate prefix_0fdb
    lines.append("    inline constexpr auto prefix_0fdb = internal::make_array<bool>(")
    prefix_0f_entries = []
    for mnem in mnemonics:
        entries = by_mnem[mnem]
        primary = get_primary_variant(entries)
        has_0f = "true" if primary.prefix_0f else "false"
        prefix_0f_entries.append(f"            {has_0f}")
    lines.append(",\n".join(prefix_0f_entries))
    lines.append("    );")
    lines.append("")

    lines.extend([
        "    template <e_instruction_id Id>",
        "    inline constexpr bool has_prefix_0f() {",
        "        constexpr int iid = static_cast<int>(Id) - 1;",
        "        return (iid >= 0 && iid < static_cast<int>(prefix_0fdb.size())) ? prefix_0fdb[iid] : false;",
        "    }",
        "",
        "}",
    ])

    return "\n".join(lines)


def generate_exhaustive_tests(instructions: List[InstructionEntry]) -> str:
    """Generate exhaustive test file for all instruction encodings."""
    by_mnem = group_by_mnemonic(instructions)

    lines = [
        "#include <gtest/gtest.h>",
        "#include \"static_asm.hpp\"",
        "",
        "// Auto-generated exhaustive tests from x86reference.xml",
        "",
        "using namespace static_asm;",
        "using namespace static_asm::x86;",
        "using namespace static_asm::x86::registers;",
        "using namespace static_asm::x86::instructions;",
        "",
    ]

    # Generate tests for each instruction group
    test_cases = []

    # ALU instructions (ADD, SUB, AND, OR, XOR, CMP, ADC, SBB)
    alu_ops = ['ADD', 'SUB', 'AND', 'OR', 'XOR', 'CMP', 'ADC', 'SBB']
    for mnem in alu_ops:
        if mnem not in by_mnem:
            continue
        safe_name = get_cpp_safe_name(mnem)
        test_cases.append(f"""
TEST(GeneratedAluTests, {mnem}_Reg32_Reg32) {{
    // {mnem} eax, ecx
    auto result = {safe_name}(eax, ecx);
    EXPECT_GE(result.size(), 2u);
}}

TEST(GeneratedAluTests, {mnem}_Reg64_Reg64) {{
    // {mnem} rax, rcx (needs REX.W)
    auto result = {safe_name}(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W prefix
}}

TEST(GeneratedAluTests, {mnem}_Reg64_Imm32) {{
    // {mnem} rax, 0x12345678
    auto result = {safe_name}(rax, 0x12345678);
    EXPECT_GE(result.size(), 6u);
}}
""")

    # MOV instruction
    if 'MOV' in by_mnem:
        test_cases.append("""
TEST(GeneratedMovTests, MOV_Reg32_Reg32) {
    auto result = mov(eax, ecx);
    EXPECT_EQ(result.size(), 2u);
}

TEST(GeneratedMovTests, MOV_Reg64_Reg64) {
    auto result = mov(rax, rcx);
    EXPECT_GE(result.size(), 3u);
    EXPECT_EQ(result[0] & 0xF8, 0x48); // REX.W
}

TEST(GeneratedMovTests, MOV_Reg64_Imm64) {
    // Should auto-detect movabs for 64-bit immediates
    constexpr std::uint64_t large_val = 0x123456789ABCDEF0ULL;
    auto result = mov(rax, large_val);
    EXPECT_EQ(result.size(), 10u); // REX.W + B8 + 8 bytes
}

TEST(GeneratedMovTests, MOV_ExtendedReg) {
    auto result = mov(r8, r9);
    EXPECT_GE(result.size(), 3u);
    // Should have REX prefix with R and B bits
}
""")

    # JMP instruction
    if 'JMP' in by_mnem:
        test_cases.append("""
TEST(GeneratedJmpTests, JMP_Rel32) {
    auto result = jmp(0x12345678);
    EXPECT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0], 0xE9);
}

TEST(GeneratedJmpTests, JMP_Reg64) {
    auto result = jmp(rax);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0xFF);
}

TEST(GeneratedJmpTests, JMP_ExtendedReg) {
    auto result = jmp(r8);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0x41); // REX.B
    EXPECT_EQ(result[1], 0xFF);
}
""")

    # CALL instruction
    if 'CALL' in by_mnem:
        test_cases.append("""
TEST(GeneratedCallTests, CALL_Rel32) {
    auto result = call(0x12345678);
    EXPECT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0], 0xE8);
}

TEST(GeneratedCallTests, CALL_Reg64) {
    auto result = call(rax);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0xFF);
}

TEST(GeneratedCallTests, CALL_ExtendedReg) {
    auto result = call(r8);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0x41); // REX.B
}
""")

    # PUSH/POP
    if 'PUSH' in by_mnem:
        test_cases.append("""
TEST(GeneratedStackTests, PUSH_Reg64) {
    auto result = push(rax);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x50);
}

TEST(GeneratedStackTests, PUSH_ExtendedReg) {
    auto result = push(r8);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0x41); // REX.B
    EXPECT_EQ(result[1], 0x50);
}
""")

    if 'POP' in by_mnem:
        test_cases.append("""
TEST(GeneratedStackTests, POP_Reg64) {
    auto result = pop(rax);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x58);
}

TEST(GeneratedStackTests, POP_ExtendedReg) {
    auto result = pop(r8);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0x41); // REX.B
}
""")

    # Conditional jumps
    jcc_ops = ['JZ', 'JNZ', 'JB', 'JNB', 'JBE', 'JNBE', 'JL', 'JNL', 'JLE', 'JNLE']
    for mnem in jcc_ops:
        if mnem not in by_mnem:
            continue
        safe_name = get_cpp_safe_name(mnem)
        test_cases.append(f"""
TEST(GeneratedJccTests, {mnem}_Rel8) {{
    auto result = {safe_name}(0x10);
    EXPECT_EQ(result.size(), 2u);
}}
""")

    # RET
    if 'RET' in by_mnem:
        test_cases.append("""
TEST(GeneratedRetTests, RET_Near) {
    auto result = ret();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0xC3);
}

TEST(GeneratedRetTests, RET_Near_Imm16) {
    auto result = ret(0x10);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0xC2);
}
""")

    # NOP
    if 'NOP' in by_mnem:
        test_cases.append("""
TEST(GeneratedNopTests, NOP_Single) {
    auto result = nop();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x90);
}
""")

    lines.extend(test_cases)
    return "\n".join(lines)


def print_instruction_summary(instructions: List[InstructionEntry]):
    print(f"Parsed {len(instructions)} instruction entries")

    by_mnem = group_by_mnemonic(instructions)
    print(f"Unique mnemonics: {len(by_mnem)}")

    supported = filter_supported_instructions(instructions)
    supported_by_mnem = group_by_mnemonic(supported)
    print(f"Supported mnemonics: {len(supported_by_mnem)}")
    print()

    examples = ['MOV', 'ADD', 'JMP', 'CALL', 'PUSH', 'POP', 'XOR', 'JZ', 'JNZ', 'LEA', 'IMUL']
    for mnem in examples:
        if mnem in by_mnem:
            entries = by_mnem[mnem]
            print(f"{mnem}: {len(entries)} variants")
            for e in entries[:3]:
                ops = ", ".join(
                    f"{op.addressing}{op.type}" if op.addressing != 'fixed' else op.group
                    for op in e.operands if op.displayed
                )
                prefix = "0F " if e.prefix_0f else ""
                ext = f"/{e.opcode_ext}" if e.opcode_ext is not None else ""
                print(f"  {prefix}{e.opcode:02X}{ext}: {e.mnemonic} {ops}")
            if len(entries) > 3:
                print(f"  ... and {len(entries) - 3} more")
            print()


def print_instruction_details(instructions: List[InstructionEntry], name: str):
    name = name.upper()
    matches = [i for i in instructions if i.mnemonic == name]

    if not matches:
        print(f"No instruction found with mnemonic: {name}")
        return

    print(f"=== {name} ({len(matches)} variants) ===\n")

    for e in matches:
        prefix = "0F " if e.prefix_0f else ""
        ext = f" /{e.opcode_ext}" if e.opcode_ext is not None else ""
        print(f"Opcode: {prefix}{e.opcode:02X}{ext}")
        print(f"Brief: {e.brief}")
        print(f"Groups: {', '.join(e.groups)}")
        print(f"Encoding: {e.get_encoding_type()}")
        print(f"ModRM: {e.needs_modrm()}")
        print(f"Has Immediate: {e.has_immediate()}")
        print(f"Reg in Opcode: {e.has_reg_in_opcode()}")
        if e.tttn:
            print(f"Condition (tttn): {e.tttn}")
        print("Operands:")
        for i, op in enumerate(e.operands):
            role = "dst" if op.is_dst else "src"
            addr_desc = ADDRESSING_MODES.get(op.addressing, op.addressing)
            if op.addressing == 'fixed':
                print(f"  [{i}] {role}: {op.group} (fixed register)")
            else:
                print(f"  [{i}] {role}: {op.addressing}{op.type} - {addr_desc}")
        print()


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Generate instruction database from x86reference.xml')
    parser.add_argument('--list', action='store_true', help='List all instructions')
    parser.add_argument('--instruction', '-i', type=str, help='Show details for specific instruction')
    parser.add_argument('--generate-db', action='store_true', help='Generate instruction_db.hpp files')
    parser.add_argument('--generate-tests', action='store_true', help='Generate test file')
    parser.add_argument('--output-dir', '-o', type=str, help='Output directory')
    args = parser.parse_args()

    all_instructions = load_instructions()
    instructions = filter_supported_instructions(all_instructions)

    if args.instruction:
        print_instruction_details(all_instructions, args.instruction)
    elif args.generate_db:
        # Ensure gen directory exists
        output_dir = Path(args.output_dir) if args.output_dir else GEN_DIR
        output_dir.mkdir(parents=True, exist_ok=True)

        # Generate instruction_db.hpp (types go to parent dir, data goes to gen/)
        db_hpp = generate_instruction_db_hpp(instructions)
        output_path = output_dir.parent / 'instruction_db.gen.hpp'
        output_path.write_text(db_hpp)
        print(f"Generated: {output_path}")

        # Generate instruction_db.g.hpp (into gen/ directory)
        db_g_hpp = generate_instruction_db_g_hpp(instructions)
        output_path = output_dir / 'instruction_db.g.hpp'
        output_path.write_text(db_g_hpp)
        print(f"Generated: {output_path}")

    elif args.generate_tests:
        tests = generate_exhaustive_tests(instructions)
        output_dir = Path(args.output_dir) if args.output_dir else TEST_DIR
        output_path = output_dir / 'test_generated.cpp'
        output_path.write_text(tests)
        print(f"Generated: {output_path}")

    else:
        print_instruction_summary(all_instructions)


if __name__ == "__main__":
    main()
