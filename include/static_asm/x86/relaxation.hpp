// static_asm - internal branch and RIP-relative relocation engine
// SPDX-License-Identifier: BSL-1.0 OR MIT
//
// The public API is in symbolic.hpp. This fixed-capacity engine receives
// already encoded instructions and typed symbolic references.
#ifndef STATIC_ASM_X86_RELAXATION_HPP
#define STATIC_ASM_X86_RELAXATION_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace static_asm::x86::detail {

    // The symbolic layer supplies exact capacities from its flattened types.
    template<std::size_t BytePool = 1024, std::size_t MaxFrag = 256, std::size_t MaxLabel = 128>
    class relaxation_engine {
    public:
        using label_id = std::size_t;

        // Mark the current position as the target for `l`.
        constexpr void bind(label_id l) {
            frags_[frag_count_++] = frag{ frag_kind::label, 0, 0, 0, false, 0, l };
        }

        // Append raw encoded bytes (e.g. the array returned by any instruction).
        template<std::size_t N>
        constexpr void put(const std::array<std::uint8_t, N>& bytes) {
            auto off = pool_count_;
            for (std::size_t i = 0; i < N; ++i) {
                pool_[pool_count_++] = bytes[i];
            }
            frags_[frag_count_++] = frag{ frag_kind::bytes, off, N, 0, false, 0, 0 };
        }

        // Append a RIP-relative instruction and resolve its disp32 to `target`.
        // The instruction must be built with a `rip + 0` placeholder operand.
        // `imm_tail` is the number of immediate bytes that follow the disp32
        // (0 for loads/stores/lea; 4 for e.g. mov [rip], imm32). The disp32 then
        // sits at offset N - 4 - imm_tail and is patched to point at `target`.
        template<std::size_t N>
        constexpr void put_rip(const std::array<std::uint8_t, N>& instr, label_id target, std::size_t imm_tail = 0) {
            static_assert(N >= 5, "a RIP-relative instruction is at least 5 bytes");
            auto off = pool_count_;
            for (std::size_t i = 0; i < N; ++i) {
                pool_[pool_count_++] = instr[i];
            }
            // imm_tail is stored in the (otherwise unused) rel8_op slot.
            frags_[frag_count_++] = frag{ frag_kind::rip_bytes, off, N, static_cast<std::uint8_t>(imm_tail), false, 0, target };
        }

        // Internal entry point for typed symbolic branch fragments.
        constexpr void put_branch(std::uint8_t rel8_op, bool rel32_0f, std::uint8_t rel32_op, label_id l) {
            branch(rel8_op, rel32_0f, rel32_op, l);
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
            if (N != total_size(st)) {
                std::abort();
            }
            std::array<std::uint8_t, N> out{};
            std::size_t pos = 0;
            for (std::size_t i = 0; i < frag_count_; ++i) {
                const frag& f = frags_[i];
                if (f.kind == frag_kind::bytes) {
                    for (std::size_t k = 0; k < f.len; ++k) {
                        out[pos++] = pool_[f.off + k];
                    }
                } else if (f.kind == frag_kind::rip_bytes) {
                    std::size_t start = pos;
                    for (std::size_t k = 0; k < f.len; ++k) {
                        out[pos++] = pool_[f.off + k];
                    }
                    // RIP displacement is relative to the end of the whole
                    // instruction (including any trailing immediate).
                    std::uint32_t disp = static_cast<std::uint32_t>(checked_disp32(st.label_off[f.target], start + f.len));
                    std::size_t d = start + f.len - 4 - f.rel8_op; // disp32 sits before the imm tail
                    out[d + 0] = static_cast<std::uint8_t>(disp & 0xFF);
                    out[d + 1] = static_cast<std::uint8_t>((disp >> 8) & 0xFF);
                    out[d + 2] = static_cast<std::uint8_t>((disp >> 16) & 0xFF);
                    out[d + 3] = static_cast<std::uint8_t>((disp >> 24) & 0xFF);
                } else if (f.kind == frag_kind::branch) {
                    std::size_t instr_end = f_offset(st, i) + frag_size(st, i);
                    std::int32_t disp = checked_disp32(st.label_off[f.target], instr_end);
                    if (!st.is_long[i]) {
                        out[pos++] = f.rel8_op;
                        out[pos++] = static_cast<std::uint8_t>(disp);
                    } else {
                        if (f.rel32_0f) {
                            out[pos++] = 0x0F;
                        }
                        out[pos++] = f.rel32_op;
                        const auto bits = static_cast<std::uint32_t>(disp);
                        out[pos++] = static_cast<std::uint8_t>(bits & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((bits >> 8) & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((bits >> 16) & 0xFF);
                        out[pos++] = static_cast<std::uint8_t>((bits >> 24) & 0xFF);
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
            std::size_t off; // bytes: start in pool_
            std::size_t len; // bytes: length
            std::uint8_t rel8_op; // branch: short opcode (0 => no short form)
            bool rel32_0f; // branch: near form has 0x0F prefix (jcc)
            std::uint8_t rel32_op; // branch: near opcode
            label_id target; // branch/label: label id
        };

        struct relax_state {
            std::array<bool, MaxFrag> is_long{}; // per-branch width choice
            std::array<std::size_t, MaxLabel> label_off{};
        };

        static constexpr std::int64_t displacement(std::size_t target, std::size_t instr_end) {
            if (target > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()) ||
                instr_end > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max())) {
                std::abort();
            }
            return static_cast<std::int64_t>(target) - static_cast<std::int64_t>(instr_end);
        }

        static constexpr std::int32_t checked_disp32(std::size_t target, std::size_t instr_end) {
            const auto disp = displacement(target, instr_end);
            if (disp < std::numeric_limits<std::int32_t>::min() || disp > std::numeric_limits<std::int32_t>::max()) {
                std::abort();
            }
            return static_cast<std::int32_t>(disp);
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
                    std::int64_t disp = displacement(st.label_off[f.target], instr_end);
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
    };

} // namespace static_asm::x86::detail

#endif // STATIC_ASM_X86_RELAXATION_HPP
