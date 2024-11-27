import sys

from PIL import Image

IMAGE_WIDTH = 32 * 2
IMAGE_HEIGHT = IMAGE_WIDTH

arduino_array = []

def to_array(path):
    img = Image.open(path).convert("RGB")
    pixels = img.load()

    for i in range(0, IMAGE_WIDTH):
        for j in range(0, IMAGE_HEIGHT):
            arduino_array.append(pixels[(i, j)])

    print("{", end="")
    for p in arduino_array:
        # print(p)
        if p == (0, 0, 0):
            print(f"0", end=",")
        elif p == (230, 141, 62):
            print(f"1", end=",")
        elif p == (13, 23, 31):
            print(f"2", end=",")
        elif p == (66, 143, 66):
            print(f"3", end=",")

    print("}", end="")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python3 png_to_array.py [path_to_image]")
        sys.exit(1)

    to_array(sys.argv[1])

