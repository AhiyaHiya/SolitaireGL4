#!/usr/bin/env bash
# =============================================================================
# PNG -> 256 px width resizer using Inkscape
#   • Takes every *.png in INPUT_DIR
#   • Exports a PNG that is exactly 256 px wide (aspect ratio preserved)
# =============================================================================

set -euo pipefail
IFS=$'\n\t'

# -------------------------------
# Configuration
# -------------------------------
INPUT_DIR="./PNG-cards-1.3"      # <-- change as needed
OUTPUT_DIR="./resized"
TARGET_WIDTH=256

# -------------------------------
# Find Inkscape
# -------------------------------
INKSCAPE=$(command -v inkscape) || {
    echo "Inkscape not found in PATH" >&2
    echo "  Debian: sudo apt install inkscape" >&2
    exit 1
}

mkdir -p "$OUTPUT_DIR"

echo "Resizing PNGs from '$INPUT_DIR' → ${TARGET_WIDTH}px width"
echo "Output folder: '$OUTPUT_DIR'"
echo

count=0

# -------------------------------
# Main loop – processes *every* .png (case-insensitive)
# -------------------------------
shopt -s nullglob nocaseglob   # make *.png match nothing if no files

for png_file in "$INPUT_DIR"/*.png; do
    # If the glob expands to the literal string "*.png" -> no files -> skip
    [[ -f "$png_file" ]] || continue

    echo "Processing file: " $png_file

    base_name=$(basename "$png_file" .png)
    output_file="$OUTPUT_DIR/${base_name}.png"

    # Skip if output is newer than source
    if [[ -f "$output_file" ]] && [[ "$png_file" -ot "$output_file" ]]; then
        echo "Skip (up-to-date): $base_name.png"
        continue
    fi

    echo "Resizing: $base_name.png"

    # ---- 2. Resize with Inkscape ----
    "$INKSCAPE" \
        --export-width="$TARGET_WIDTH" \
        --export-filename="$output_file" \
        "$png_file" \
        >/dev/null 2>&1

done

echo
echo "All done."
echo "Resized PNGs are in: $OUTPUT_DIR"
