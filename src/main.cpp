// New feature! Overclocking WS2812
// #define FASTLED_OVERCLOCK 1.2 // 20% overclock ~ 960 khz.
#include <FastLED.h>
#define NUM_LEDS_PER_WHEEL 40 * 2

const int TOTAL_LEDS = NUM_LEDS_PER_WHEEL * NUM_LEDS_PER_WHEEL;

CRGB grid[TOTAL_LEDS];

CRGB get_grid_colour(int x, int y) {
    int pos_1d = get_1d_arr_index(x, y);

    // this shouldn't happen
    if (pos_1d > NUM_LEDS_PER_WHEEL) {
        return CRGB::Red;
    }

    return grid[pos_1d];
}

void set_grid_colour(int x, int y, CRGB colour) {
    int pos_1d = get_1d_arr_index(x, y);
    grid[pos_1d] = colour;
}

int get_1d_arr_index(int x, int y) {
    // map the x,y coords to positive array indices
    int x_translated = x + NUM_LEDS_PER_WHEEL / 2;
    int y_translated = y + NUM_LEDS_PER_WHEEL / 2;

    // flip y coord so the image isn't upside down
    y_translated = NUM_LEDS_PER_WHEEL - y_translated;

    return (y_translated * NUM_LEDS_PER_WHEEL) + x_translated;
}

void setup_pizza_slices() {
    for (int i = (-NUM_LEDS_PER_WHEEL / 2); i < NUM_LEDS_PER_WHEEL / 2; i++) {
        for (int j = NUM_LEDS_PER_WHEEL / 2; j < -NUM_LEDS_PER_WHEEL / 2; j--) {
            if (i < 0 && j > 0) {
                set_grid_colour(i, j, CRGB::Green);
            } else if (i > 0 && j > 0) {
                set_grid_colour(i, j, CRGB::Blue);
            } else if (i < 0 && j < 0) {
                set_grid_colour(i, j, CRGB::Red);
            } else if (i > 0 && j < 0) {
                set_grid_colour(i, j, CRGB::White);
            }
        }
    }
}

setup_pizza_slices();