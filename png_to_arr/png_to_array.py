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
        if p == (0, 0, 0):
            print(f"0", end=",")
        else:
            print(f"1", end=",")
    print("}", end="")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python3 png_to_array.py [path_to_image]")
        sys.exit(1)

    to_array(sys.argv[1])

