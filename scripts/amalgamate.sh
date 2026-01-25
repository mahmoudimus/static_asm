#!/bin/bash
# Generate single-header amalgamation of static_asm

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_FILE="$ROOT_DIR/single_include/static_asm.hpp"

# Get version from git tag or use date
VERSION=$(git describe --tags 2>/dev/null || date +%Y.%m.%d)

# Create output directory
mkdir -p "$(dirname "$OUTPUT_FILE")"

# Create header
HEADER="// static_asm - Compile-time x86/x86-64 assembler for C++20
// Version: $VERSION
// https://github.com/mahmoudimus/static_asm
//
// SPDX-License-Identifier: BSL-1.0 OR MIT
//
// Licensed under either:
//   - Boost Software License 1.0 (https://www.boost.org/LICENSE_1_0.txt)
//   - MIT License (https://opensource.org/licenses/MIT)
//
// This is a single-header amalgamation of the static_asm library.
// For the modular source files, see the repository.
//
// Original work by Midi12 (https://github.com/Midi12)
// Extended by Mahmoud Abdelkader (https://github.com/mahmoudimus)
//

"

# Generate amalgamation using quom
TEMP_FILE=$(mktemp)
quom "$ROOT_DIR/include/static_asm.hpp" "$TEMP_FILE" \
    -I "$ROOT_DIR/include" \
    -I "$ROOT_DIR/src" \
    --trim

# Combine header and amalgamated content
echo "$HEADER" > "$OUTPUT_FILE"
cat "$TEMP_FILE" >> "$OUTPUT_FILE"
rm "$TEMP_FILE"

# Report
LINES=$(wc -l < "$OUTPUT_FILE")
echo "Generated: $OUTPUT_FILE"
echo "Version:   $VERSION"
echo "Lines:     $LINES"
