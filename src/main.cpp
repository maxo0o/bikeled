// New feature! Overclocking WS2812
// #define FASTLED_OVERCLOCK 1.2 // 20% overclock ~ 960 khz.
#include <FastLED.h>
#define NUM_LEDS 32
#define DATA_PIN 8
#define DATA_PIN2 9
#define BRIGHTNESS 255

CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];

int inPin = A0; 
void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN2>(leds2, NUM_LEDS);
    pinMode(inPin, INPUT);
    Serial.begin(115200);
    FastLED.setBrightness(BRIGHTNESS);
}

int pos = 0;
int dot = 0;
bool latch = false;
void loop() {
    float outputV = analogRead(inPin) * 5000.0 / 1023;
    // Serial.print("Output Voltaje = ");
    // Serial.print(outputV);
    // Serial.print(" mV   \n");

    if (outputV > 3250 && latch == false) {
        pos++;
        if (pos > 1) pos = 0;
        latch = true;
    } else if (outputV < 3220) {
        latch = false;
    }

    // if (pos == 0) {
        for(int led = 0; led < NUM_LEDS; led++) { 
            leds[led] = CRGB::Black;
            leds2[led] = CRGB::Black;
        }
        leds[dot] = CRGB::Red;
        leds2[dot] = CRGB::Red;
        FastLED.show();
        delay(50);

        if (dot > NUM_LEDS) dot = 0;
        dot++;

        // for(int led = 0; led < NUM_LEDS; led++) { 
        //     leds[led] = CRGB::Red;
        //     leds2[led] = CRGB::Red;
        //     FastLED.show();
        //     delay(50);
        // }

    // } else if (pos == 1) {
    //     for(int led = 0; led < NUM_LEDS; led++) { 
    //         leds[led] = CRGB::Blue;
    //         leds2[led] = CRGB::Blue;
    //         FastLED.show();
    //     }
    // }
    // } else if (pos == 2) {
    //     for(int led = 0; led < NUM_LEDS; led++) { 
    //         leds[led] = CRGB::Green;
    //     }
    //     FastLED.show();

    // } else {
    //     for(int led = 0; led < NUM_LEDS; led++) { 
    //         leds[led] = CRGB::Red;
    //     }
    //     FastLED.show();
    // }
}