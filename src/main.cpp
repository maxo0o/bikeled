// New feature! Overclocking WS2812
// #define FASTLED_OVERCLOCK 1.2 // 20% overclock ~ 960 khz.

// LEDS
#include <FastLED.h>
#include <main.h>
#include <Math.h>
#define NUM_LEDS 32
#define DATA_PIN 8
#define NUM_LEDS_PER_WHEEL 32

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

const int TOTAL_LEDS = (NUM_LEDS_PER_WHEEL * 2) * (NUM_LEDS_PER_WHEEL * 2);

CRGB grid[TOTAL_LEDS];

int _get_1d_arr_index(int x, int y) {
    // map the x,y coords to positive array indices
    int x_translated = x + NUM_LEDS_PER_WHEEL;
    int y_translated = y + NUM_LEDS_PER_WHEEL;

    // flip y coord so the image isn't upside down
    // y_translated = NUM_LEDS_PER_WHEEL - y_translated;

    // Serial.println("x trans:");
    // Serial.println(x_translated);
    // Serial.println("y trans:");
    // Serial.println(y_translated);

    return (y_translated * NUM_LEDS_PER_WHEEL) + x_translated;
}

CRGB get_grid_colour(int x, int y) {
    int pos_1d = _get_1d_arr_index(x, y);

    Serial.println("pos_1d:");
    Serial.println(pos_1d);
    // this shouldn't happen
    if (pos_1d > TOTAL_LEDS) {
        return CRGB::Green;
    }

    return grid[pos_1d];
}

void set_grid_colour(int x, int y, CRGB colour) {
    int pos_1d = _get_1d_arr_index(x, y);
    grid[pos_1d] = colour;
}

void setup_pizza_slices() {
    for (int i = (-NUM_LEDS_PER_WHEEL / 2); i < NUM_LEDS_PER_WHEEL / 2; i++) {
        for (int j = NUM_LEDS_PER_WHEEL / 2; j > -NUM_LEDS_PER_WHEEL / 2; j--) {
            if (i < 0 && j > 0) {
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

void triangle_art() {
    int points = 1;
    for (int i = (-NUM_LEDS_PER_WHEEL / 2); i < NUM_LEDS_PER_WHEEL / 2; i++) {
        for (int j = NUM_LEDS_PER_WHEEL / 2; j > -NUM_LEDS_PER_WHEEL / 2; j--) {
            if () {
                set_grid_colour(j, i, CRGB::White);
            } else {
                set_grid_colour(j, i, CRGB::Black);
            }
        }
    }
}

// void loop() {
//     for (int i = 0; i < NUM_LEDS_PER_WHEEL; i++) {
//         for (int j = 0; j < NUM_LEDS_PER_WHEEL; j++) {
//             CRGB c = grid[(j * NUM_LEDS_PER_WHEEL) + i];
//             if (c == CRGB::Blue) {
//                 Serial.write("B, ");
//             } else if (c == CRGB::Red) {
//                 Serial.write("R, ");
//             } else if (c == CRGB::Green) {
//                 Serial.write("G, ");
//             } else {
//                 Serial.write("Y, ");
//             }
//         }
//         Serial.write("\n");
//     }
//     Serial.write("\n");
// }


void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    pinMode(inPin, INPUT);
    Serial.begin(115200);
    setup_pizza_slices();
}

// void ledMagic(int x_pos, int y_pos) {
//   // given a specified x,y pos, get the polar coordinates
//   // if the strips theta matches those x,y, coords, then light up the LED at ledN (specific radius)
//   r = sqrt16((x_pos*x_pos) + (y_pos*y_pos));  
//   led_n = r * (ledsPerMeter * wheelRadius);
//   desired_theta = atan(y_pos / x_pos);

//   led_n = 8;
//   if ((abs((real_theta*1000) - (desired_theta*1000)) / 1000) < 0.02) {
//     leds[led_n] = CRGB::DarkGoldenrod;
//   } else {
//       for(int led = 0; led < NUM_LEDS; led++) {
//           leds[led] = CRGB::Black;
//       }
//   }
//   FastLED.show();
// }

void ledMagic() {
  // given a specified x,y pos, get the polar coordinates
  // if the strips theta matches those x,y, coords, then light up the LED at ledN (specific radius)
  int x = 15 * cos(real_theta);
  int y = 15 * sin(real_theta);

  Serial.write("Pos: \n");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print("\n");

  for(int led = 0; led < NUM_LEDS; led++) {
    leds[led] = get_grid_colour(x, y);
    // leds[led] = CRGB::Blue;
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

    ledMagic();
    // ledMagic(35, 35);
    // ledMagic(15, 15);
}
