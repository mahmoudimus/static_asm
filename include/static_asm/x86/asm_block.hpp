// static_asm - relaxation-based assembler block (labels + branch resolution)
// SPDX-License-Identifier: BSL-1.0 OR MIT
//
// PROTOTYPE. This is the "assembler" layer on top of the pure encoder: it
// resolves branch targets to labels and size-optimizes each branch (rel8 when
// the target fits in +/-127, rel32 otherwise) via iterative relaxation, all at
// compile time.
//
// It also closes the runtime-patch-offset gap: a bound label records the exact
// final byte offset of whatever follows it, so a patch site is just a label.
//
//   using namespace static_asm::x86;
//   constexpr auto code = build([](asm_block& b) {
//       auto loop = b.label();
//       b.bind(loop);
//       b.put(dec(ecx));        // any encoded instruction array
//       b.jne(loop);            // -> 75 FD  (rel8 chosen automatically)
//   });
//
// Patch-offset example:
//   constexpr auto prog = build([](asm_block& b) {
//       auto ctx = b.label();
//       b.bind(ctx);            // ctx now marks the start of the next bytes
//       b.put(movabs(rax, 0));  // imm64 placeholder to patch at runtime
//   });
//   constexpr std::size_t imm_off = prog.offset_of(ctx) + 2; // skip 48 B8
#ifndef STATIC_ASM_X86_ASM_BLOCK_HPP
#define STATIC_ASM_X86_ASM_BLOCK_HPP

#include <array>
#include <cstddef>
#include <cstdint>

// The typed label helpers (lea/mov to a label) build real instructions, so pull
// in the operand types, registers and instruction functions.
#include "gen/instruction.g.hpp"
#include "operands.hpp"

namespace static_asm::x86 {

    // A compile-time assembler block with a fixed capacity (no heap, so the
    // whole block is a structural type usable as a `constexpr` value). The
    // defaults are generous for shellcode-sized routines; bump them for larger
    // programs.
    template<std::size_t BytePool = 1024, std::size_t MaxFrag = 256, std::size_t MaxLabel = 128>
    class asm_block {
    public:
        using label_id = std::uint16_t;

        // Allocate a fresh (unbound) label. Bind it later with bind().
        constexpr label_id label() {
            return static_cast<label_id>(label_count_++);
        }

        // Mark the current position as the target for `l`.
        constexpr void bind(label_id l) {
            frags_[frag_count_++] = frag{ frag_kind::label, 0, 0, 0, false, 0, l };
        }

        // Append raw encoded bytes (e.g. the array returned by any instruction).
        template<std::size_t N>
        constexpr void put(const std::array<std::uint8_t, N>& bytes) {
            auto off = static_cast<std::uint16_t>(pool_count_);
            for (std::size_t i = 0; i < N; ++i) {
                pool_[pool_count_++] = bytes[i];
            }
            frags_[frag_count_++] = frag{ frag_kind::bytes, off, static_cast<std::uint16_t>(N), 0, false, 0, 0 };
        }

        // Append a RIP-relative instruction and resolve its disp32 to `target`.
        // The instruction must be built with a `rip + 0` placeholder operand.
        // `imm_tail` is the number of immediate bytes that follow the disp32
        // (0 for loads/stores/lea; 4 for e.g. mov [rip], imm32). The disp32 then
        // sits at offset N - 4 - imm_tail and is patched to point at `target`.
        //     auto data = b.label();
        //     b.put_rip(lea(rax, qword_ptr(rip + 0)), data);          // imm_tail 0
        //     b.put_rip(mov(dword_ptr(rip + 0), 0x7B), data, 4);      // imm32 tail
        //     ...
        //     b.bind(data);
        //     b.dq(0xCAFEBABE);
        template<std::size_t N>
        constexpr void put_rip(const std::array<std::uint8_t, N>& instr, label_id target, std::size_t imm_tail = 0) {
            static_assert(N >= 5, "a RIP-relative instruction is at least 5 bytes");
            auto off = static_cast<std::uint16_t>(pool_count_);
            for (std::size_t i = 0; i < N; ++i) {
                pool_[pool_count_++] = instr[i];
            }
            // imm_tail is stored in the (otherwise unused) rel8_op slot.
            frags_[frag_count_++] = frag{ frag_kind::rip_bytes, off, static_cast<std::uint16_t>(N), static_cast<std::uint8_t>(imm_tail), false, 0, target };
        }

        // Data directives: emit raw little-endian data. Bind a label before one
        // of these to reference the data (e.g. via put_rip).
        constexpr void db(std::uint8_t v) {
            put(std::array<std::uint8_t, 1>{ v });
        }
        constexpr void dw(std::uint16_t v) {
            put(std::array<std::uint8_t, 2>{ static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8) });
        }
        constexpr void dd(std::uint32_t v) {
            put(std::array<std::uint8_t, 4>{ static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8),
                static_cast<std::uint8_t>(v >> 16), static_cast<std::uint8_t>(v >> 24) });
        }
        constexpr void dq(std::uint64_t v) {
            put(std::array<std::uint8_t, 8>{ static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8),
                static_cast<std::uint8_t>(v >> 16), static_cast<std::uint8_t>(v >> 24),
                static_cast<std::uint8_t>(v >> 32), static_cast<std::uint8_t>(v >> 40),
                static_cast<std::uint8_t>(v >> 48), static_cast<std::uint8_t>(v >> 56) });
        }

        // Typed RIP-relative label helpers (sugar over put_rip): load/store a
        // register from/to a labelled location, or take its address.
        //     b.lea(rax, data);   // lea  rax, [rip+data]
        //     b.mov(rax, data);   // mov  rax, [rip+data]   (load)
        //     b.mov(data, rax);   // mov  [rip+data], rax   (store)
        template<typename Reg>
            requires Register<Reg>
        constexpr void lea(const Reg& reg, label_id target) {
            put_rip(instructions::lea(reg, qword_ptr(registers::rip + 0)), target);
        }
        template<typename Reg>
            requires Register<Reg>
        constexpr void mov(const Reg& reg, label_id target) { // load
            put_rip(instructions::mov(reg, rip_mem0<Reg>()), target);
        }
        template<typename Reg>
            requires Register<Reg>
        constexpr void mov(label_id target, const Reg& reg) { // store
            put_rip(instructions::mov(rip_mem0<Reg>(), reg), target);
        }

        // Branches to a (possibly forward) label. Width is chosen by relaxation.
        constexpr void jmp(label_id l) {
            branch(0xEB, false, 0xE9, l);
        }
        constexpr void jo(label_id l) {
            cc(0x0, l);
        }
        constexpr void jno(label_id l) {
            cc(0x1, l);
        }
        constexpr void jb(label_id l) {
            cc(0x2, l);
        }
        constexpr void jae(label_id l) {
            cc(0x3, l);
        }
        constexpr void jz(label_id l) {
            cc(0x4, l);
        }
        constexpr void jnz(label_id l) {
            cc(0x5, l);
        }
        constexpr void je(label_id l) {
            cc(0x4, l);
        }
        constexpr void jne(label_id l) {
            cc(0x5, l);
        }
        constexpr void jbe(label_id l) {
            cc(0x6, l);
        }
        constexpr void ja(label_id l) {
            cc(0x7, l);
        }
        constexpr void js(label_id l) {
            cc(0x8, l);
        }
        constexpr void jns(label_id l) {
            cc(0x9, l);
        }
        constexpr void jl(label_id l) {
            cc(0xC, l);
        }
        constexpr void jge(label_id l) {
            cc(0xD, l);
        }
        constexpr void jle(label_id l) {
            cc(0xE, l);
        }
        constexpr void jg(label_id l) {
            cc(0xF, l);
        }
        // CALL rel has no rel8 form; always rel32.
        constexpr void call(label_id l) {
            branch(0x00, false, 0xE8, l);
        }

        // Total encoded size after relaxation.
        constexpr std::size_t size() const {
            relax_state st{};
            relax(st);
            return total_size(st);
        }

        // Final byte offset of a bound label (valid after relaxation).
        constexpr std::size_t offset_of(label_id l) const {
            relax_state st{};
            relax(st);
            return st.label_off[l];
        }

        // Materialize the encoded bytes. N must equal size().
        template<std::size_t N>
        constexpr std::array<std::uint8_t, N> assemble() const {
            relax_state st{};
            relax(st);
            std::array<std::uint8_t, N> out{};
            std::size_t pos = 0;
            for (std::size_t i = 0; i < frag_count_; ++i) {
                const frag& f = frags_[i];
                if (f.kind == frag_kind::bytes) {
                    for (std::uint16_t k = 0; k < f.len; ++k) {
                        out[pos++] = pool_[f.off + k];
                    }
                } else if (f.kind == frag_kind::rip_bytes) {
                    std::size_t start = pos;
                    for (std::uint16_t k = 0; k < f.len; ++k) {
                        out[pos++] = pool_[f.off + k];
                    }
                    // RIP displacement is relative to the end of the whole
                    // instruction (including any trailing immediate).
                    std::int32_t disp = static_cast<std::int32_t>(st.label_off[f.target]) - static_cast<std::int32_t>(start + f.len);
                    std::size_t d = start + f.len - 4 - f.rel8_op; // disp32 sits before the imm tail
                    out[d + 0] = static_cast<std::uint8_t>(disp & 0xFF);
                    out[d + 1] = static_cast<std::uint8_t>((disp >> 8) & 0xFF);
                    out[d + 2] = static_cast<std::uint8_t>((disp >> 16) & 0xFF);
                    out[d + 3] = static_cast<std::uint8_t>((disp >> 24) & 0xFF);
                } else if (f.kind == frag_kind::branch) {
                    std::size_t instr_end = f_offset(st, i) + frag_size(st, i);
                    std::int32_t disp = static_cast<std::int32_t>(st.label_off[f.target]) - static_cast<std::int32_t>(instr_end);
                    if (!st.is_long[i]) {
                        out[pos++] = f.rel8_op;
                        out[pos++] = static_cast<std::uint8_t>(static_cast<std::int8_t>(disp));
                    } else {
                        if (f.rel32_0f) {
                            out[pos++] = 0x0F;
                        }
                        out[pos++] = f.rel32_op;
                        out[pos++] = static_cast<std::uint8_t>(disp & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((disp >> 8) & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((disp >> 16) & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((disp >> 24) & 0xFF);
                    }
                }
                // label frags contribute no bytes
            }
            return out;
        }

    private:
        enum class frag_kind : std::uint8_t { bytes,
            rip_bytes,
            branch,
            label };

        struct frag {
            frag_kind kind;
            std::uint16_t off; // bytes: start in pool_
            std::uint16_t len; // bytes: length
            std::uint8_t rel8_op; // branch: short opcode (0 => no short form)
            bool rel32_0f; // branch: near form has 0x0F prefix (jcc)
            std::uint8_t rel32_op; // branch: near opcode
            std::uint16_t target; // branch/label: label id
        };

        struct relax_state {
            std::array<bool, MaxFrag> is_long{}; // per-branch width choice
            std::array<std::size_t, MaxLabel> label_off{};
        };

        // A [rip + 0] memory operand sized to match Reg (for load/store).
        template<typename Reg>
        static constexpr auto rip_mem0() {
            if constexpr (Reg::size == 8) {
                return byte_ptr(registers::rip + 0);
            } else if constexpr (Reg::size == 16) {
                return word_ptr(registers::rip + 0);
            } else if constexpr (Reg::size == 32) {
                return dword_ptr(registers::rip + 0);
            } else {
                return qword_ptr(registers::rip + 0);
            }
        }

        constexpr void cc(std::uint8_t code, label_id l) {
            // jcc: short 0x70+cc (1 byte), near 0x0F 0x80+cc (2 bytes)
            branch(static_cast<std::uint8_t>(0x70 + code), true, static_cast<std::uint8_t>(0x80 + code), l);
        }

        constexpr void branch(std::uint8_t rel8_op, bool rel32_0f, std::uint8_t rel32_op, label_id l) {
            frags_[frag_count_++] = frag{ frag_kind::branch, 0, 0, rel8_op, rel32_0f, rel32_op, l };
        }

        constexpr std::size_t rel32_len(const frag& f) const {
            return (f.rel32_0f ? 2u : 1u) + 4u;
        }

        constexpr std::size_t frag_size(const relax_state& st, std::size_t i) const {
            const frag& f = frags_[i];
            if (f.kind == frag_kind::bytes || f.kind == frag_kind::rip_bytes) {
                return f.len; // fixed size (RIP-relative instructions never relax)
            }
            if (f.kind == frag_kind::branch) {
                return st.is_long[i] ? rel32_len(f) : 2u;
            }
            return 0; // label
        }

        constexpr std::size_t f_offset(const relax_state& st, std::size_t upto) const {
            std::size_t off = 0;
            for (std::size_t i = 0; i < upto; ++i) {
                off += frag_size(st, i);
            }
            return off;
        }

        constexpr std::size_t total_size(const relax_state& st) const {
            std::size_t off = 0;
            for (std::size_t i = 0; i < frag_count_; ++i) {
                off += frag_size(st, i);
            }
            return off;
        }

        // Iterate to a fixpoint. Branches start short and only ever promote to
        // long (monotonic), so this terminates in at most frag_count_ passes.
        constexpr void relax(relax_state& st) const {
            // CALL and any branch with no short form start long.
            for (std::size_t i = 0; i < frag_count_; ++i) {
                if (frags_[i].kind == frag_kind::branch && frags_[i].rel8_op == 0) {
                    st.is_long[i] = true;
                }
            }
            bool changed = true;
            while (changed) {
                changed = false;
                // recompute label offsets under current widths
                std::size_t off = 0;
                for (std::size_t i = 0; i < frag_count_; ++i) {
                    if (frags_[i].kind == frag_kind::label) {
                        st.label_off[frags_[i].target] = off;
                    }
                    off += frag_size(st, i);
                }
                // check each short branch; promote if it no longer fits rel8
                for (std::size_t i = 0; i < frag_count_; ++i) {
                    const frag& f = frags_[i];
                    if (f.kind != frag_kind::branch || st.is_long[i]) {
                        continue;
                    }
                    std::size_t instr_end = f_offset(st, i) + 2u; // short size
                    std::int32_t disp = static_cast<std::int32_t>(st.label_off[f.target]) - static_cast<std::int32_t>(instr_end);
                    if (disp < -128 || disp > 127) {
                        st.is_long[i] = true;
                        changed = true;
                    }
                }
            }
        }

        std::array<std::uint8_t, BytePool> pool_{};
        std::size_t pool_count_ = 0;
        std::array<frag, MaxFrag> frags_{};
        std::size_t frag_count_ = 0;
        std::size_t label_count_ = 0;
    };

    // Build a block from a callable and return the finished bytes. The size is
    // computed from the same block, so N is never stated by hand.
    template<typename F>
    constexpr auto build(F builder) {
        constexpr std::size_t n = [builder] {
            asm_block<> b;
            builder(b);
            return b.size();
        }();
        asm_block<> b;
        builder(b);
        return b.template assemble<n>();
    }

} // namespace static_asm::x86

#endif // STATIC_ASM_X86_ASM_BLOCK_HPP
