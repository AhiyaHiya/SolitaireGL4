#!/usr/bin/env python3
"""
Create a PNG atlas + JSON from a folder of PNGs.
Requires beyond Pillow, `pip install --user pillow`
"""
import json
import math
import sys
from pathlib import Path
from PIL import Image

# ----------------------------------------------------------------------
# CONFIG
# ----------------------------------------------------------------------
INPUT_DIR   = Path("./cards")      # <-- folder with 256‑px‑wide PNGs
OUTPUT_PNG  = Path("cards.png")
OUTPUT_JSON = Path("cards.json")
MAX_SIZE    = 4096                   # atlas max dimension
PADDING     = 2                      # pixels between sprites

# ----------------------------------------------------------------------
def pack_images(image_paths):
    images = [Image.open(p).convert("RGBA") for p in image_paths]
    w = max(img.width for img in images)
    h = max(img.height for img in images)

    # Simple shelf packing - good enough for 52 cards of same size
    per_row = math.floor((MAX_SIZE - PADDING) / (w + PADDING))
    rows    = math.ceil(len(images) / per_row)

    atlas_w = per_row * (w + PADDING) + PADDING
    atlas_h = rows    * (h + PADDING) + PADDING
    atlas_w = min(atlas_w, MAX_SIZE)
    atlas_h = min(atlas_h, MAX_SIZE)

    atlas = Image.new("RGBA", (atlas_w, atlas_h), (0, 0, 0, 0))

    frames = {}
    x = y = PADDING
    for idx, img in enumerate(images):
        name = image_paths[idx].name
        atlas.paste(img, (x, y))

        frames[name] = {
            "x": x,
            "y": y,
            "w": img.width,
            "h": img.height,
        }

        x += w + PADDING
        if (idx + 1) % per_row == 0:
            x = PADDING
            y += h + PADDING

    return atlas, frames

# ----------------------------------------------------------------------
def main():
    if not INPUT_DIR.is_dir():
        print(f"Error: {INPUT_DIR} not found", file=sys.stderr)
        sys.exit(1)

    pngs = sorted(INPUT_DIR.glob("*.png"))
    if not pngs:
        print("No PNGs found", file=sys.stderr)
        sys.exit(1)

    print(f"Packing {len(pngs)} images -> {OUTPUT_PNG}")
    atlas_img, frames = pack_images(pngs)

    atlas_img.save(OUTPUT_PNG, optimize=True)
    with open(OUTPUT_JSON, "w") as f:
        json.dump({"frames": frames}, f, indent=2)

    print(f"Done -> {OUTPUT_PNG}  ({atlas_img.width}x{atlas_img.height})")
    print(f"JSON -> {OUTPUT_JSON}")

if __name__ == "__main__":
    main()
