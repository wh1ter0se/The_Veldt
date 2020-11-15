#include <FastLED.h>

#define LED_PIN  6
#define HOTEND_LED_PIN 7
#define COMMS_PIN 4
#define NUM_LEDS 36
#define HOTEND_NUM_LEDS 16
#define NUM_STRIPS 2
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_STRIPS][NUM_LEDS];

const int COLUMN_LIGHTS[] = {0, 1, 16, 17, 18, 19, 34, 35};
const int COLUMN_LIGHTS_LENGTH = 8;
const int MASKED_LIGHTS[] = {2, 15, 20, 33};
const int MASKED_LIGHTS_LENGTH = 4;

long rainbowPosition;

void setup() {
  delay(1500); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds[0], NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, HOTEND_LED_PIN, COLOR_ORDER>(leds[1], HOTEND_NUM_LEDS).setCorrection( TypicalLEDStrip );

  pinMode(LED_PIN, OUTPUT);
  pinMode(COMMS_PIN, INPUT);
  rainbowPosition = 0;
}

void loop() {
  
  if (digitalRead(COMMS_PIN) == LOW) {
    //rainbow(30, 255, 0);
    //doubleSolidRainbow(20, 255, 18, 85);
    //twoPillars_SolidRainbow(30, 255, 200);
    twoPillars_SolidRainbow_WithHotend(30,255,190);
  } else {
    off();
  }
  
}

void rainbow(int speed, int brightness, double pitch){ // TODO direction
  rainbowPosition++;
  if (rainbowPosition > 255) { rainbowPosition = 0; }

  for(int i=0; i< NUM_LEDS; i++) {
    leds[0][i].setHSV(rainbowPosition + (pitch * i), 255, brightness);
    //leds[i] = ColorFromPalette( currentPalette, rainbowPosition, 255, currentBlending);
  }
  FastLED.show();
  if (rainbowPosition == 255) { for (int i=0;i<5;i++) { delay(speed); } }
  delay(speed); 
}

void doubleSolidRainbow(int speed, int brightness, int startSecondSection, int offset) {
  rainbowPosition++;
  if (rainbowPosition > 255) { rainbowPosition = 0; }

  for(int i=0; i< startSecondSection; i++) {
    leds[0][i].setHSV(rainbowPosition, 255, brightness);
  }
  for(int i=startSecondSection; i< NUM_LEDS; i++) {
    leds[0][i].setHSV(rainbowPosition + offset, 255, brightness);
  }
  FastLED.show();
  if (rainbowPosition == 255) { for (int i=0;i<5;i++) { delay(speed); } }
  delay(speed); 
}

void twoPillars_SolidRainbow(int speed, int brightness, int offset) {
  rainbowPosition++;
  if (rainbowPosition > 255) { rainbowPosition = 0; }
  
  for (int i=0;i<NUM_LEDS;i++) {
    leds[0][i].setHSV(rainbowPosition, 255, brightness);
  }
  for (int i=0;i<COLUMN_LIGHTS_LENGTH/2;i++) {
    leds[0][COLUMN_LIGHTS[i]].setHSV((int)rainbowPosition + offset, 255, brightness);
  }
  for (int i=0;i<MASKED_LIGHTS_LENGTH/2;i++) {
    leds[0][MASKED_LIGHTS[i]].setHSV((int)rainbowPosition + (offset / 2), 255, 0);
  }
  FastLED.show();
  delay(speed); 
}

void twoPillars_SolidRainbow_WithHotend(int speed, int brightness, int offset) {
  rainbowPosition++;
  if (rainbowPosition > 255) { rainbowPosition = 0; }
  
  for (int i=0;i<NUM_LEDS;i++) {
    leds[0][i].setHSV(rainbowPosition, 255, brightness);
  }
  for (int i=0;i<COLUMN_LIGHTS_LENGTH/2;i++) {
    leds[0][COLUMN_LIGHTS[i]].setHSV((int)rainbowPosition - offset, 255, brightness);
  }
  for (int i=0;i<MASKED_LIGHTS_LENGTH/2;i++) {
    leds[0][MASKED_LIGHTS[i]].setHSV((int)rainbowPosition + offset, 255, 0);
  }
  for (int i=0;i<HOTEND_NUM_LEDS;i++) {
    leds[1][i].setHSV((int)rainbowPosition + offset, 255, brightness);
  }
  FastLED.show();
  delay(speed); 
}

// Turns every LED off
void off() {
  FastLED.clear();  // clear all pixel data
  FastLED.show();
}