#!/usr/bin/env python3
"""Fetch prebuilt raylib into third_party/raylib/ (gitignored, reproducible).

Usage: python tools/fetch_raylib.py [--proxy http://127.0.0.1:7897]
Only stdlib. Version pinned in RAYLIB_RELEASE; bump deliberately (ADR-0002).
"""
import argparse
import io
import sys
import urllib.request
import zipfile
from pathlib import Path

RAYLIB_RELEASE = "5.5"
RAYLIB_URL = (
    "https://github.com/raysan5/raylib/releases/download/"
    f"{RAYLIB_RELEASE}/raylib-{RAYLIB_RELEASE}_win64_msvc16.zip"
)
DEST = Path(__file__).resolve().parent.parent / "third_party" / "raylib"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--proxy", default=None, help="http proxy, e.g. http://127.0.0.1:7897")
    args = ap.parse_args()

    marker = DEST / f".raylib-{RAYLIB_RELEASE}-ok"
    if marker.exists():
        print(f"raylib {RAYLIB_RELEASE} already present at {DEST}")
        return 0

    handlers = []
    if args.proxy:
        handlers.append(urllib.request.ProxyHandler({"http": args.proxy, "https": args.proxy}))
    opener = urllib.request.build_opener(*handlers)

    print(f"downloading {RAYLIB_URL} ...")
    with opener.open(RAYLIB_URL, timeout=120) as resp:
        blob = resp.read()
    print(f"downloaded {len(blob) // 1024} KiB")

    DEST.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(io.BytesIO(blob)) as zf:
        for info in zf.infolist():
            name = Path(info.filename)
            parts = name.parts[1:]  # strip top-level folder
            if not parts or info.is_dir():
                continue
            target = DEST.joinpath(*parts)
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(zf.read(info))

    lib = DEST / "lib" / "raylib.lib"
    header = DEST / "include" / "raylib.h"
    if not lib.exists() or not header.exists():
        print(f"FATAL: expected {lib} and {header} after extract", file=sys.stderr)
        return 1
    marker.write_text(RAYLIB_RELEASE)
    print(f"ok: {lib}")
    print(f"ok: {header}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
