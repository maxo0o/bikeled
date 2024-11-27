// New feature! Overclocking WS2812
// #define FASTLED_OVERCLOCK 1.2 // 20% overclock ~ 960 khz.

// LEDS
#include <FastLED.h>
#include <main.h>
#include <Math.h>

#define NUM_LEDS 32
#define DATA_PIN 8
#define LED_ROW_SIZE NUM_LEDS * 2

CRGB leds[NUM_LEDS];

// Hall effect sensor
#define THRESHOLD 3230
int inPin = A0;
bool halleffectTriggered = false;

//Wheel speed
volatile unsigned long lastPulseTime = 0;
volatile unsigned long currentPulseTime = 0;
volatile float currentTime = 0;

//coords systems
volatile float omega = 0;
volatile float real_theta = 0;

volatile float x_pos = 0;
volatile float y_pos = 0;
volatile float desired_theta = 0;

volatile int r = 0; // radius based on x, y coords
volatile int led_n = 0; // led based on the radius

const int magnets = 1;
const float wheelRadius = 0.26;
const float ledsPerMeter = 144;

const int TOTAL_LEDS = LED_ROW_SIZE * LED_ROW_SIZE;

CRGB grid[TOTAL_LEDS];

unsigned long art_timer = millis();

int _get_1d_arr_index(int x, int y) {
    // map the x,y coords to positive array indices
    int x_translated = x + LED_ROW_SIZE / 2;
    int y_translated = y + LED_ROW_SIZE / 2;

    // flip y coord so the image isn't upside down
    // y_translated = LED_ROW_SIZE - y_translated;

    return (y_translated * LED_ROW_SIZE) + x_translated;
}

CRGB get_grid_colour(int x, int y) {
    int pos_1d = _get_1d_arr_index(x, y);

    // Serial.println("Pos:");
    // Serial.print(pos_1d);
    // Serial.println();

    // this shouldn't happen
    if (pos_1d > TOTAL_LEDS) {
        return CRGB::Green;
    }

    return grid[pos_1d];
}

void clear_grid(CRGB clear_colour) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        grid[i] = clear_colour;
    }
}

void set_grid_colour(int x, int y, CRGB colour) {
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
                set_grid_colour(i, j, CRGB::Red);
            } else if (i > 0 && j >= 0) {
                set_grid_colour(i, j, CRGB::Blue);
            } else if (i <= 0 && j <= 0) {
                set_grid_colour(i, j, CRGB::White);
            } else if (i >= 0 && j <= 0) {
                set_grid_colour(i, j, CRGB::Yellow);
            }
        }
    }
}

int square_size = 1;
int square_inc = 1;
void square(int size) {
    clear_grid(CRGB::Black);

    for (int i = -size; i <= size; i++) {
        for (int j = -size; j <= size; j++) {
            set_grid_colour(i, j, CRGB::Red);
        }
    }
}

void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    pinMode(inPin, INPUT);
    Serial.begin(115200);

    pizza_slices();
    // square(10);
}

void ledMagic() {
  for(int led = 0; led < NUM_LEDS; led++) {
    int x = (led + 0) * cos(real_theta);
    int y = (led + 0) * sin(real_theta);

    // Serial.println("Pos:");
    // Serial.print(x);
    // Serial.print(", ");
    // Serial.print(y);
    // Serial.println();

    leds[led] = get_grid_colour(x, y);
  }
  FastLED.show();
}

void loop() {
    volatile float timeBetweenPulses;
    volatile float timeSincePulse;

    //check to see if hall effect sensor has triggered, i.e. 1 rotation
    float outputV = analogRead(inPin) * 5000.0 / 1023;
    if (outputV > THRESHOLD && !halleffectTriggered) {
        lastPulseTime = currentPulseTime;
        currentPulseTime = micros();
        halleffectTriggered = true;
        currentTime = 0;
    } else if (outputV <= THRESHOLD) {
        halleffectTriggered = false;
    }

    //calc speed
    timeBetweenPulses = ((currentPulseTime - lastPulseTime));
    omega =  ((2 * 3.14) / (timeBetweenPulses)) * 1000000;
    timeSincePulse = (micros() - currentPulseTime); //us
    real_theta = (omega * timeSincePulse) / 1000000;

    // unsigned long current_millis = millis();
    // if (current_millis - art_timer > 1000) {
    //     if (square_size == 0) square_inc = 1;
    //     if (square_size > 16) square_inc = -1;

    //     square_size += square_inc;
    //     square(square_size);
    // }

    ledMagic();

    // grid_debug();
}
