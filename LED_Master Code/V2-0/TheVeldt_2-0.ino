#include <FastLED.h>

////////// PINS //////////
#define BACKSPLASH_LED_PIN 6
#define DESK_LED_PIN       9
#define BED_LED_PIN        10
#define SKYHALO_LED_PIN    11

////////// STRIP CONSTANTS/VARS //////////
#define BACKSPLASH_NUM_LEDS 84 // real 84
    const int arcOneSize = 47;
    int arcOne[arcOneSize] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
    const int arcTwoSize = 16;
    int arcTwo[arcTwoSize] = {47,48,49,50,51,52,53,54,55,56,57,58,80,81,82,83};
    const int arcThreeSize = 21;
    int arcThree[arcThreeSize] = {59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79};
#define DESK_NUM_LEDS       30  // TODO update when desk underglow is installed
#define BED_NUM_LEDS        35  // TODO update when bed underglow is installed
#define SKYHALO_NUM_LEDS    92 // TODO update when skyhalo is installed, real 92


#define NUM_STRIPS 4
#define LARGEST_STRIP_NUM_LEDS 92

const int NUM_LEDS[NUM_STRIPS] {BACKSPLASH_NUM_LEDS, DESK_NUM_LEDS, BED_NUM_LEDS, SKYHALO_NUM_LEDS};

////////// FASTLED VARS //////////
CRGB leds[NUM_STRIPS][LARGEST_STRIP_NUM_LEDS];
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGBPalette16 currentPalette  = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;

////////// SYNCRONIZING VARIABLES //////////
int displayMode;
long primaryTicker = 0;
long initMillis;

////////// PALETTES //////////
DEFINE_GRADIENT_PALETTE( dark_to_bright_gp ) {
   0,0,0,0,
   128,128,128,128,
   255,255,255,255};

void setup() {
    // Setup any uninstantiated init vars
    // for (int i=0;i<NUM_STRIPS;i++) {ALL_STRIP_IDS[i] = i;}
    FastLED.addLeds<LED_TYPE, BACKSPLASH_LED_PIN, COLOR_ORDER>(leds[0], BACKSPLASH_NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, DESK_LED_PIN, COLOR_ORDER>(leds[1], DESK_NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, BED_LED_PIN, COLOR_ORDER>(leds[2], BED_NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, SKYHALO_LED_PIN, COLOR_ORDER>(leds[3], SKYHALO_NUM_LEDS).setCorrection( TypicalLEDStrip );

    // Pin modes
    pinMode(BACKSPLASH_LED_PIN, OUTPUT);
    pinMode(DESK_LED_PIN, OUTPUT);
    pinMode(BED_LED_PIN, OUTPUT);
    pinMode(SKYHALO_LED_PIN, OUTPUT);

    // Set display mode
    displayMode = 1;

    Serial.begin(19200);
}

void loop() {
  //currentPalette = northern_lights_gp;
  Serial.println("Loop");
  int defaultOffset;
  int defaultHue;
  int defaultSaturation;
  int defaultBrightness;
    int finalBrightness;
  int defaultSinVal;
  int defaultSkew;
  int defaultColor;
  switch (displayMode) {
    case 0: // Off
      FastLED.clear();
      FastLED.show();
      break;
    case 1: // Displays a synced/offset solidRainbow across everything
      defaultOffset = 20;
      defaultBrightness = 200;
      tripleArc_SolidRainbow(defaultOffset, defaultBrightness);
      //backsplash_SolidRainbow(defaultOffset, defaultBrightness);
      desk_SolidRainbow(-1 * defaultOffset, defaultBrightness);
      bed_SolidRainbow(-1 * defaultOffset, defaultBrightness);
      skyhalo_SolidRainbow(defaultOffset * 2, defaultBrightness);
      //skyhalo_SolidRainbow(0, 200);
      delay(50);
      tick(1,0,255);
      break;
    case 2: // Displays a rainbow across everything
      currentPalette  = RainbowColors_p;
      defaultSkew = 3;
      defaultBrightness = 255;
      defaultSinVal = 10;
      backsplash_PaletteFade(defaultSkew, defaultBrightness, defaultSinVal);
      desk_PaletteFade(defaultSkew, defaultBrightness, defaultSinVal);
      bed_PaletteFade(defaultSkew, defaultBrightness, defaultSinVal);
      skyhalo_PaletteFade(defaultSkew, defaultBrightness, defaultSinVal);
      delay(0);
      tick(3,0,255);
      break;
    case 3: // Night light to alarm clock 
      FastLED.clear();
      FastLED.show();
      //defaultColor = 95;
      //defaultBrightness = 8;
      //desk_PaletteColor(defaultColor,0);
      //bed_PaletteColor(defaultColor,defaultBrightness);
      alarmClock(300);
      displayMode = 1;
      break;
    case 4: // Violet-Blue Solid Glow
      defaultColor = 85;
      defaultBrightness = 255;
      backsplash_PaletteColor(defaultColor,defaultBrightness);
      desk_PaletteColor(defaultColor,defaultBrightness);
      bed_PaletteColor(defaultColor,defaultBrightness);
      skyhalo_PaletteColor(defaultColor,defaultBrightness);
      break;
    // case 5: // Violet-Blue Solid Underglow
    //   defaultColor = 0;
    //   int contrastColor = 210;
    //   defaultBrightness = 127;
    //   int contrastBrightness = 255;
    //   backsplash_PaletteColor(defaultColor,defaultBrightness);
    //   desk_PaletteColor(defaultColor,defaultBrightness);
    //   bed_PaletteColor(defaultColor,defaultBrightness);
    //   skyhalo_PaletteColor(contrastColor,contrastBrightness);
    //   break;
    // case 6: // 90-minute Sunrise
    //   defaultBrightness = 180;
    //   backsplash_HSV(0,255,defaultBrightness);
    //   desk_HSV(0,255,defaultBrightness);
    //   bed_HSV(0,255,defaultBrightness);
    //   fill_solid(leds[0], NUM_LEDS[0], CHSV(225,195,defaultBrightness));
    //   fill_solid(leds[1], NUM_LEDS[1], CHSV(225,195,defaultBrightness));
    //   fill_solid(leds[2], NUM_LEDS[2], CHSV(225,195,defaultBrightness));
    //   FastLED.show();
    //   tick(1,0,255);
    //   break;
    case 5:
      defaultHue        = 225;
      defaultSaturation = 175;
      defaultBrightness = 225;
      backsplash_HSV(defaultHue, defaultSaturation, defaultBrightness);
      desk_HSV(defaultHue, defaultSaturation, defaultBrightness);
      bed_HSV(defaultHue, defaultSaturation, defaultBrightness);
      //skyHalo_HSV(defaultHue, defaultSaturation, 255);
      skyhalo_PaletteColor(230,255);
      break;
    case 6: // Red Glow
      defaultColor = 0;
      defaultBrightness = 255;
      backsplash_PaletteColor(defaultColor,defaultBrightness);
      desk_PaletteColor(defaultColor,defaultBrightness);
      bed_PaletteColor(defaultColor,defaultBrightness);
      skyhalo_PaletteColor(defaultColor,defaultBrightness);
      break;
    case 7: // Orange Glow
      defaultColor = 230;
      defaultBrightness = 255;
      backsplash_PaletteColor(defaultColor,defaultBrightness);
      desk_PaletteColor(defaultColor,defaultBrightness);
      bed_PaletteColor(defaultColor,defaultBrightness);
      skyhalo_PaletteColor(defaultColor,defaultBrightness);
      //skyHalo_HSV(225,175,255);
      break;
  }
}

// Modifies the primaryTicker
void tick(int diff, int min, int max) {
  primaryTicker += diff;
  if (primaryTicker < min) { primaryTicker = max; }
  if (primaryTicker > max) { primaryTicker = min; }
}

void tripleArc_SolidRainbow(int offset, int brightness) {
  for (int i=0;i<arcOneSize;i++) {
    leds[0][arcOne[i]] = ColorFromPalette(currentPalette, primaryTicker + offset, brightness, currentBlending);
  }
  for (int i=0;i<arcTwoSize;i++) {
    leds[0][arcTwo[i]] = ColorFromPalette(currentPalette, primaryTicker, brightness, currentBlending);
  }
  for (int i=0;i<arcThreeSize;i++) {
    leds[0][arcThree[i]] = ColorFromPalette(currentPalette, primaryTicker - offset, brightness, currentBlending);
  }
  FastLED.show();
}

void backsplash_SolidRainbow(int offset, int brightness) {
  for (int i=0;i<BACKSPLASH_NUM_LEDS;i++) {
    leds[0][i] = ColorFromPalette(currentPalette, primaryTicker + offset, brightness, currentBlending);
  }
  FastLED.show();
}

void desk_SolidRainbow(int offset, int brightness) {
  for (int i=0;i<DESK_NUM_LEDS;i++) {
    leds[1][i] = ColorFromPalette(currentPalette, primaryTicker + offset, brightness, currentBlending);
  }
  FastLED.show();
}

void bed_SolidRainbow(int offset, int brightness) {
  for (int i=0;i<BED_NUM_LEDS;i++) {
    leds[2][i] = ColorFromPalette(currentPalette, primaryTicker + offset, brightness, currentBlending);
  }
  FastLED.show();
}

void skyhalo_SolidRainbow(int offset, int brightness) {
  for (int i=0;i<SKYHALO_NUM_LEDS;i++) {
    leds[3][i] = ColorFromPalette(currentPalette, primaryTicker + offset, brightness, currentBlending);
  }
  FastLED.show();
}

void backsplash_PaletteFade(double skew, int brightness, int sinVal) {
  for (int i=0;i<BACKSPLASH_NUM_LEDS;i++) {
    leds[0][i] = ColorFromPalette(currentPalette, (primaryTicker + (int)(skew * i) + int(sinVal * sin((primaryTicker-i)*((double)(2*M_PI / 255)))))%255, brightness, currentBlending);
  }
  FastLED.show();
}

void desk_PaletteFade(double skew, int brightness, int sinVal) {
  for (int i=0;i<DESK_NUM_LEDS;i++) {
    leds[1][i] = ColorFromPalette(currentPalette, (primaryTicker + (int)(skew * i) + int(sinVal * sin((primaryTicker-i)*((double)(2*M_PI / 255)))))%255, brightness, currentBlending);
  }
  FastLED.show();
}

void bed_PaletteFade(double skew, int brightness, int sinVal) {
  for (int i=0;i<BED_NUM_LEDS;i++) {
    leds[2][i] = ColorFromPalette(currentPalette, (primaryTicker + (int)(skew * i) + int(sinVal * sin((primaryTicker-i)*((double)(2*M_PI / 255)))))%255, brightness, currentBlending);
  }
  FastLED.show();
}

void skyhalo_PaletteFade(double skew, int brightness, int sinVal) {
  for (int i=0;i<SKYHALO_NUM_LEDS;i++) {
    leds[3][i] = ColorFromPalette(currentPalette, (primaryTicker + (int)(skew * i) + int(sinVal * sin((primaryTicker-i)*((double)(2*M_PI / 255)))))%255, brightness, currentBlending);
  }
  FastLED.show();
}

void backsplash_PaletteColor(int position, int brightness) {
  for (int i=0;i<BACKSPLASH_NUM_LEDS;i++) {
    leds[0][i] = ColorFromPalette(currentPalette, position, brightness, currentBlending);
  }
  FastLED.show();
}

void desk_PaletteColor(int position, int brightness) {
  for (int i=0;i<DESK_NUM_LEDS;i++) {
    leds[1][i] = ColorFromPalette(currentPalette, position, brightness, currentBlending);
  }
  FastLED.show();
}

void bed_PaletteColor(int position, int brightness) {
  for (int i=0;i<BED_NUM_LEDS;i++) {
    leds[2][i] = ColorFromPalette(currentPalette, position, brightness, currentBlending);
  }
  FastLED.show();
}

void skyhalo_PaletteColor(int position, int brightness) {
  for (int i=0;i<SKYHALO_NUM_LEDS;i++) {
    leds[3][i] = ColorFromPalette(currentPalette, position, brightness, currentBlending);
  }
  FastLED.show();
}

void backsplash_HSV(int H, int S, int V) {
  fill_solid(leds[0], BACKSPLASH_NUM_LEDS, CHSV(H,S,V));
  FastLED.show();
}

void bed_HSV(int H, int S, int V) {
  fill_solid(leds[1], BED_NUM_LEDS, CHSV(H,S,V));
  FastLED.show();
}

void desk_HSV(int H, int S, int V) {
  fill_solid(leds[2], DESK_NUM_LEDS, CHSV(H,S,V));
  FastLED.show();
}

void skyHalo_HSV(int H, int S, int V) {
  fill_solid(leds[3], SKYHALO_NUM_LEDS, CHSV(H,S,V));
  FastLED.show();
}

void alarmClock(int minutes) {
  long delayMillis = minutes * 60000;
  delay(delayMillis);
}