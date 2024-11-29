// New feature! Overclocking WS2812
// #define FASTLED_OVERCLOCK 1.2 // 20% overclock ~ 960 khz.

// LEDS
#include <FastLED.h>
#include <main.h>
#include <Math.h>

CRGB leds[NUM_LEDS];
CRGB leds_b[NUM_LEDS];

// Hall effect sensor
#define THRESHOLD 3230
int inPin_halleffect0 = A0;
int inPin_halleffect1 = A1;

volatile bool halleffectTriggered = false;

//Wheel speed
volatile unsigned long lastPulseTime = 0;
volatile unsigned long currentPulseTime = 0;
volatile float currentTime = 0;

//coords systems
volatile float omega = 0;
volatile float real_theta = 0;

const byte ALPHA_BYTE = 0;

byte grid[TOTAL_LEDS]; 
unsigned long art_timer = millis();
int frame_counter = 1;

int _get_1d_arr_index(int x, int y) {
    // map the x,y coords to positive array indices
    int x_translated = x + LED_ROW_SIZE / 2;
    int y_translated = y + LED_ROW_SIZE / 2;
    y_translated = LED_ROW_SIZE - y_translated;

    // flip y coord so the image isn't upside down
    // y_translated = LED_ROW_SIZE - y_translated;

    return (y_translated * LED_ROW_SIZE) + x_translated;
}

CRGB get_grid_colour(int x, int y) {
    int pos_1d = _get_1d_arr_index(x, y);

    // this shouldn't happen
    if (pos_1d > TOTAL_LEDS) {
        return CRGB::Green;
    }

    int c = grid[pos_1d];

    if (c == 0) {
        return CRGB(100, 100, 100);
    }

    // Serial.println(c);

    return colour_map[c];
}

void clear_grid(byte clear_colour) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        grid[i] = clear_colour;
    }
}

void set_grid_colour(int x, int y, byte colour) {
    int pos_1d = _get_1d_arr_index(x, y);
    grid[pos_1d] = colour;
}

void grid_debug() {
    for (int i = 0; i < LED_ROW_SIZE; i++) {
        for (int j = 0; j < LED_ROW_SIZE; j++) {
            CRGB c = grid[(i * LED_ROW_SIZE) + j];
            if (c == CRGB::Red) {
                Serial.write("R, ");
            } else if (c == CRGB::Blue) {
                Serial.write("B, ");
            } else if (c == CRGB::White) {
                Serial.write("W, ");
            } else if (c == CRGB::Yellow) {
                Serial.write("Y, ");
            } else {
                Serial.write("*, ");
            }
        }
        Serial.write("\n");
    }
    Serial.write("\n");
}

void pizza_slices() {
    for (int i = (-LED_ROW_SIZE / 2); i < LED_ROW_SIZE / 2; i++) {
        for (int j = (-LED_ROW_SIZE / 2); j < LED_ROW_SIZE / 2; j++) {
            if (i <= 0 && j >= 0) {
                set_grid_colour(i, j, 1);
            } else if (i > 0 && j >= 0) {
                set_grid_colour(i, j, 9);
            } else if (i <= 0 && j <= 0) {
                set_grid_colour(i, j, 9);
            } else if (i >= 0 && j <= 0) {
                set_grid_colour(i, j, 1);
            }
        }
    }
}

// int square_size = 1;
// int square_inc = 1;
// void square(int size) {
//     clear_grid(CRGB::Black);

//     for (int i = -size; i <= size; i++) {
//         for (int j = -size; j <= size; j++) {
//             set_grid_colour(i, j, CRGB::Red);
//         }
//     }
// }

void line() {
    clear_grid(0);

        for (int j = 0; j < LED_ROW_SIZE / 2; j++) {
            set_grid_colour(0, j, 2);
            set_grid_colour(1, j, 2);
        }
}

void load_image() {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        byte c = pgm_read_word_near(star + i);
        if (c != ALPHA_BYTE) grid[i] = c;
    }
}

// CRGB get_s_eye(int _x, int _y) {
//     const int row_size = 156;

//     int i = _x * 2;
//     int j = _y * 2;

//     i = i + row_size / 2;
//     j = j + row_size / 2;

//     byte c = pgm_read_word_near(soccer + ((j * row_size) + i));
//     int p = c;
//     CRGB colour_average = colour_map[p];

//     int positions[9][2] = {
//         {i - 1, j},     // left
//         {i - 1, j - 1}, // left-top
//         {i, j - 1},     // top
//         {i + 1, j - 1}, // right-top
//         {i + 1, j},     // right 
//         {i + 1, j + 1}, // right-bottom
//         {i, j + 1},     // bottom
//         {i - 1, j + 1}, // left-bottom
//     };

//     for (int p = 0; p < 9; p++) {
//         int x = positions[p][0];
//         int y = positions[p][1];

//         if (0 < x && y < row_size) {
//             byte c = pgm_read_word_near(soccer + ((y * row_size) + x));
//             int p = c;
//             // colour_average = CRGB::blend(colour_average, CRGB(CRGB::Green), 0.5);

//             int r = (CRGB(colour_average).r + CRGB(colour_map[p]).r) / 2;
//             int g = (CRGB(colour_average).g + CRGB(colour_map[p]).g) / 2;
//             int b = (CRGB(colour_average).b + CRGB(colour_map[p]).b) / 2;

//             colour_average = CRGB(r, g, b);
//         }
//     }

//     return colour_average;
// }

// void dog(int frame) {
//     if (frame == 0) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_1 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     } else if (frame == 1) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_2 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     } else if (frame == 2) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_3 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     } else if (frame == 3) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_4 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     } else if (frame == 4) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_5 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     } else if (frame == 5) {
//         for (int i = 0; i < TOTAL_LEDS; i++) {
//             byte c = pgm_read_word_near(dog_6 + i);
//             if (c != ALPHA_BYTE) grid[i] = c;
//         }
//     }
// }


void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN_A>(leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN_B>(leds_b, NUM_LEDS);

    pinMode(inPin_halleffect0, INPUT);
    pinMode(inPin_halleffect1, INPUT);

    Serial.begin(115200);

    FastLED.setBrightness(65);

    load_image();

    // pizza_slices();
    // square(10);
    // iyoni();
    // canary();
    // dog(0);
    // cat();
    // load_inyoni();
    // line();
    // load_inyoni();
    // load_eye();
}

void checkHES() { //check hall effect sensor

    volatile float timeBetweenPulses;
    volatile float timeSincePulse;

    //check to see if hall effect sensor has triggered, i.e. 1 rotation
    float outputV = analogRead(inPin_halleffect0) * 5000.0 / 1023;
    // float outputV1 = analogRead(inPin_halleffect1) * 5000.0 / 1023;

    if ( (outputV > THRESHOLD) && !halleffectTriggered) {
        lastPulseTime = currentPulseTime;
        currentPulseTime = micros();
        halleffectTriggered = true;
        currentTime = 0;
    } else if (outputV <= THRESHOLD) {
        halleffectTriggered = false;
    }

    //calc speed
    timeBetweenPulses = ((currentPulseTime - lastPulseTime));
    omega =  ((2 * PI) / (timeBetweenPulses)) * 1000000;
    timeSincePulse = (micros() - currentPulseTime); //us
    real_theta = (omega * timeSincePulse) / 1000000;
}


void ledMagic() {
  for(int led = 0; led < NUM_LEDS; led++) {
    
    checkHES();
    int x = (led + DEAD_ZONE_LED_SIZE - 1) * cos(real_theta + PI / 2);
    int y = (led + DEAD_ZONE_LED_SIZE - 1) * sin(real_theta + PI / 2);
    leds[led] = get_grid_colour(x, y);
    // leds[led] = get_s_eye(x, y);

    // checkHES();
    // x = (led + DEAD_ZONE_LED_SIZE - 1) * cos(real_theta + PI);
    // y = (led + DEAD_ZONE_LED_SIZE - 1) * sin(real_theta + PI);
    // leds_b[led] = get_grid_colour(x, y);

  }
  FastLED.show();
}

int tick = 0;

CRGB colours[7] = {
    CRGB::HotPink,
    CRGB::Purple,
    CRGB::Blue,
    CRGB::Green,
    CRGB::Yellow,
    CRGB::Orange,
    CRGB::Red, 
};

void loop() {
    checkHES();

    for (int l = 0; l < NUM_LEDS; l++) {
        int led = l;

        if (led < 5) {
            leds[led] = colours[(0 + tick) % 7];
            leds_b[led]  = colours[(0 + tick) % 7];
        } else if (led < 10) {
            leds[led] = colours[(1 + tick) % 7];
            leds_b[led] = colours[(1 + tick) % 7];
        }  else if (led < 15) {
            leds[led] = colours[(2 + tick) % 7];
            leds_b[led] = colours[(2 + tick) % 7];
        } else if (led < 20) {
            leds[led] = colours[(3 + tick) % 7];
            leds_b[led] = colours[(3 + tick) % 7];
        } else if (led < 25) {
            leds[led] = colours[(4 + tick) % 7];
            leds_b[led] = colours[(4 + tick) % 7];
        } else if (led < 30) {
            leds[led] = colours[(5 + tick) % 7];
            leds_b[led] = colours[(5 + tick) % 7];
        } else if (led < 35) {
            leds[led] = colours[(6 + tick) % 7];
            leds_b[led] = colours[(6 + tick) % 7];
        } 
    } 

    FastLED.show();

    // ledMagic();

    unsigned long current_art_time = millis();
    if (current_art_time - art_timer > 100) {
        tick++;
        if (tick == 7) tick = 0;
        art_timer = current_art_time;
    }

    // grid_debug();
}