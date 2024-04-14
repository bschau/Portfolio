#!/usr/bin/env python3
""" imgsize - find size of image(s). """
import sys
from PIL import Image


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: imgsize.py file1 ... fileN")
        sys.exit(0)

    if len(sys.argv) == 2:
        img = Image.open(sys.argv[1])
        w, h = img.size
        print(f"{w} x {h}")
    else:
        for arg in sys.argv[1:]:
            img = Image.open(arg)
            w, h = img.size
            print(f"{arg}: {w} x {h}")
