import sys

from PIL import Image

IMAGE_WIDTH = 39 * 2
# IMAGE_WIDTH = 156
IMAGE_HEIGHT = IMAGE_WIDTH

color_map = [
    None, # placeholder for alpha channel == 1 (transparency)
    "FFFFFF",
    "FF0000",
    "800080",
    "DB7093",
    "FFA500",
    "000080",
    "008000",
    "B8860B",
    "000000",
    "0000FF",
    "A9A9A9",
    "800000",
    "8B0000",
]


arduino_array = []


def closest_colour(color):
    distance = 9999
    index = 0
    for i in range(0, len(color_map)):
        c = color_map[i]

        if c is None: continue # ignore alpha channel placeholder

        rgb = tuple(int(c[i:i+2], 16) for i in (0, 2, 4))
        d = ((color[0] - rgb[0]) ** 2 + (color[1] - rgb[1]) ** 2 + (color[2] - rgb[2]) ** 2) ** 0.5

        if d < distance:
            distance = d
            index = i
    
    return index


def to_array(path):
    img = Image.open(path).convert("RGB")
    pixels = img.load()
    alpha = img.convert('RGBA').split()[-1].load()

    for i in range(0, IMAGE_WIDTH):
        for j in range(0, IMAGE_HEIGHT):
            arduino_array.append((pixels[(i, j)], alpha[(i, j)]))

    print("{", end="")
    for p in arduino_array:
        # print(p)
        rgb, alpha = p

        if alpha == 0:
            print(f"0", end=",")
        else:
            print(f"{closest_colour(rgb)}", end=",")
        # print(f"CRGB{rgb}", end=",")

    print("}", end=",")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python3 png_to_array.py [path_to_image]")
        sys.exit(1)

    to_array(sys.argv[1])

