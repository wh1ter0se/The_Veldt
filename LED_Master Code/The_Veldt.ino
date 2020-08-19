#include <FastLED.h>

////////// PINS //////////
#define SKYHALO_LED_PIN 6
#define PRINTER_COMM_PIN 4
#define MODE_INPUT_PIN A0

////////// STRIP CONSTANTS/VARS //////////

#define SKYHALO_NUM_LEDS 100 // TODO put this back to 50

#define NUM_STRIPS 1
#define LARGEST_STRIP_NUM_LEDS 50

const int NUM_LEDS[NUM_STRIPS] { SKYHALO_NUM_LEDS };
int ALL_STRIP_IDS[NUM_STRIPS];
CRGB leds[NUM_STRIPS][LARGEST_STRIP_NUM_LEDS];

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGBPalette16 currentPalette  = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;

////////// SYNCRONIZING VARIABLES //////////
int displayMode;
long primaryTicker = 0;
long initMillis;

////////// BLOB VARIABLES //////////
#define MAX_BLOBS 10
#define defaultBlobCount = 5;
int blobCount = 0;
int blob_Data[MAX_BLOBS][3];
    // [n]    = blob ID
    // [n][0] = strip ID
    // [n][1] = blob radius
    // [n][2] = blob center
    // [n][3] = blob velocity
    // [n][4] = blob palette position
int blob_LEDS[NUM_STRIPS][SKYHALO_NUM_LEDS];

////////// FIRE VARIABLES //////////
int fireTemps[NUM_STRIPS][LARGEST_STRIP_NUM_LEDS];
    // [n]    = strip id
    // [n][k] = pixel ID
int realFireTemps[NUM_STRIPS][LARGEST_STRIP_NUM_LEDS];

////////// PALETTES //////////
DEFINE_GRADIENT_PALETTE( aug_sunrise_gp ) {
    0,  33, 22, 13,
    0,  74, 36, 16,
    1,  87, 42, 17,
    3,  83, 45, 18,
    4,  80, 47, 19,
    4,  84, 51, 21,
    5,  90, 55, 22,
    7,  94, 57, 23,
    8,  98, 60, 23,
    9, 106, 65, 24,
    9, 112, 68, 26,
   10, 117, 71, 27,
   12, 121, 74, 27,
   13, 128, 78, 29,
   13, 132, 81, 29,
   14, 133, 84, 30,
   15, 135, 87, 31,
   17, 139, 91, 33,
   18, 142, 95, 34,
   18, 146,100, 34,
   19, 150,104, 36,
   21, 155,108, 37,
   22, 161,114, 37,
   23, 169,118, 40,
   23, 173,122, 41,
   24, 177,125, 42,
   26, 179,130, 43,
   27, 182,135, 47,
   27, 184,138, 49,
   28, 188,142, 52,
   29, 190,146, 55,
   31, 192,151, 57,
   32, 194,156, 61,
   32, 194,159, 64,
   33, 197,164, 67,
   35, 194,166, 69,
  205, 103,127,155,
  207, 110,125,153,
  208, 117,125,149,
  208, 125,124,147,
  209, 135,124,144,
  211, 148,124,140,
  212, 161,122,137,
  213, 171,121,133,
  213, 182,121,128,
  214, 192,121,123,
  216, 203,119,119,
  217, 213,119,114,
  217, 217,118,109,
  218, 220,117,108,
  219, 220,114,106,
  221, 222,114,105,
  222, 224,114,102,
  222, 224,112,100,
  223, 222,111, 99,
  225, 217,109, 99,
  226, 208,107,100,
  227, 199,105,102,
  227, 190,104,105,
  228, 182,104,106,
  230, 175,103,106,
  231, 167,101,108,
  231, 159,101,109,
  232, 152,101,111,
  233, 144,100,112,
  235, 139,100,112,
  236, 135, 99,114,
  236, 133, 99,114,
  237, 132, 99,114,
  239, 128, 99,115,
  240, 125, 97,115,
  241, 120, 97,117,
  241, 112, 97,119,
  242, 104, 97,120,
  244,  95, 97,122,
  245,  87, 97,125,
  245,  80, 97,127,
  246,  75, 97,128,
  247,  69, 96,128,
  249,  67, 95,128,
  250,  66, 95,128,
  250,  66, 95,128,
  251,  66, 93,127,
  253,  65, 95,125,
  254,  64, 93,125,
  255,  63, 92,123};
DEFINE_GRADIENT_PALETTE( oct_sunset_gp ) {
    0, 252,215, 92,
    2, 252,213, 91,
    4, 252,213, 91,
    6, 252,213, 91,
    8, 252,211, 89,
   10, 252,211, 88,
   12, 252,209, 88,
   14, 252,209, 87,
   16, 252,207, 87,
   18, 252,205, 88,
   20, 252,203, 88,
   22, 252,203, 88,
   24, 252,203, 89,
   26, 252,201, 91,
   28, 252,201, 91,
   30, 252,197, 91,
   32, 252,195, 92,
   34, 252,193, 92,
   36, 252,191, 93,
   38, 252,189, 95,
   40, 252,187, 95,
   42, 252,186, 95,
   44, 252,186, 96,
   46, 252,184, 96,
   48, 252,184, 96,
   50, 252,182, 98,
   52, 252,180, 99,
   54, 252,180, 98,
   56, 252,178,100,
   58, 252,176,100,
   60, 252,176,100,
   62, 252,176,102,
   64, 249,175,102,
   66, 249,175,103,
   68, 249,173,103,
   70, 244,173,103,
  156, 135,138,114,
  158, 133,138,115,
  160, 132,136,114,
  162, 128,136,114,
  164, 128,135,114,
  166, 126,135,114,
  168, 125,133,114,
  170, 123,133,114,
  172, 121,133,114,
  174, 120,131,114,
  176, 118,130,114,
  178, 118,130,114,
  180, 117,130,114,
  182, 115,128,112,
  184, 115,128,112,
  186, 113,127,111,
  188, 112,127,111,
  190, 110,125,112,
  192, 110,125,111,
  194, 110,124,111,
  196, 109,124,111,
  198, 107,122,111,
  200, 107,122,111,
  202, 106,122,111,
  204, 106,121,109,
  206, 104,121,109,
  208, 103,119,109,
  210, 103,119,109,
  212, 100,118,109,
  214, 100,118,109,
  216,  98,118,109,
  218,  98,117,109,
  220,  95,117,109,
  222,  95,115,109,
  224,  94,114,109,
  226,  92,114,109,
  228,  91,112,109,
  230,  90,112,109,
  232,  88,112,109,
  234,  87,112,109,
  236,  86,111,111,
  238,  86,111,111,
  240,  84,111,111,
  242,  83,109,109,
  244,  82,109,111,
  246,  82,109,109,
  248,  80,108,109,
  250,  80,108,109,
  252,  79,107,109,
  255,  79,107,108};
DEFINE_GRADIENT_PALETTE( gringrow_gp ) {
    0,  72,199, 16,
   33,  12, 96, 72,
   73,  92, 19,127,
   99,  51, 65,115,
  122,  26, 93, 95,
  137,   1, 68, 54,
  158,  72,193, 15,
  193,  51, 69,115,
  214,  82, 18,105,
  237,   1, 68, 54,
  255,   1, 68, 54};
DEFINE_GRADIENT_PALETTE( wave_gp ) {
    0, 255, 22, 16,
   42,  88,104,  0,
   84,  14,255, 16,
  127,   0,104, 92,
  170,  14, 22,255,
  212,  88,  0, 92,
  255, 255, 22, 16};
DEFINE_GRADIENT_PALETTE( occ028_gp ) {
    0,  42, 73,133,
  255, 213,127, 45};
DEFINE_GRADIENT_PALETTE( warm_colors_gp ) {
   0,255,0,0,
   125,255,0,15,
   255,255,0,60};
DEFINE_GRADIENT_PALETTE( passionata_gp ) {
    0, 217,  4,102,
   66, 217,  4, 61,
  122, 217,  4,  5,
  186, 161,  4,219,
  255,  78,  4,219};
DEFINE_GRADIENT_PALETTE( blacklight_gp ) {
  0,255,255,0,
  255,110,255,0};
DEFINE_GRADIENT_PALETTE( northern_lights_gp ) {
  0,0,00,255,
  40,0,140,200,
  120,0,0,0,
  134,0,0,0,
  215,60,135,0,
  255,255,176,0};
DEFINE_GRADIENT_PALETTE( warm2cool_gp ) {
   0,255,200,200, // warmest
   31,255,143,197,
   63,255,170,214,
   95,255,224,241,
   127,255,244,250,
   159,255,251,255,
   191,255,255,255, // strip's natural color
   223,201,255,226,
   255,64,255,156}; // coldest

void setup() {
  Serial.begin(19200);
  Serial.println("I exist.");
  displayMode = 7;                                                                                                                                             // displayMode
  FastLED.addLeds<LED_TYPE, SKYHALO_LED_PIN, COLOR_ORDER>(leds[0], SKYHALO_NUM_LEDS).setCorrection( TypicalLEDStrip ); //.setCorrection( TypicalLEDStrip )
  for (int i=0;i<NUM_STRIPS;i++) { 
    ALL_STRIP_IDS[i] = i;
    fireTemps[i][0] = -1;
  }
  for (int i=0;i<MAX_BLOBS;i++) { blob_Data[i][0] = -1; }
  pinMode(MODE_INPUT_PIN, INPUT);
  pinMode(PRINTER_COMM_PIN, OUTPUT);
  initMillis = millis();
}

void loop() {
  //currentPalette = northern_lights_gp;
  if (displayMode == 0 || displayMode == 2) {
    digitalWrite(PRINTER_COMM_PIN, HIGH);
  } else {
    digitalWrite(PRINTER_COMM_PIN, LOW);
  }
  switch (displayMode) {
    case 0: // Off
      off();
      break;
    case 1: // Displays a synced solidRainbow across all passed strips
      solidRainbow(NUM_STRIPS, ALL_STRIP_IDS);
      break;
    case 2: // Sets my morning alarm clock to a given display mode
      alarmClock(420,8);
      break;
    case 3: // Displays blobs across all initialized strips
      blobs(3);
      break;
    case 4: // Plots current palette
      plotPalette(NUM_STRIPS, ALL_STRIP_IDS,0);
      break;
    case 5: // Solid cycle through the current palette
      solidPaletteFade(NUM_STRIPS, ALL_STRIP_IDS);
      break;
    case 6: // Random flicking through the current palette
      fireFromPalette(NUM_STRIPS, ALL_STRIP_IDS, 255, 100, 1, 50);
      break;
    case 7: // Cycles through a rainbow with a gradient across a variable range
      paletteFade(NUM_STRIPS, ALL_STRIP_IDS, -1.25, 255, 0);
      break;
    case 8:
      daylight(NUM_STRIPS, ALL_STRIP_IDS, 255);
      break;
  }
}

// Modifies the primaryTicker
void tick(int diff, int min, int max) {
  primaryTicker += diff;
  if (primaryTicker < min) { primaryTicker = max; }
  if (primaryTicker > max) { primaryTicker = min; }
}

// Turns every LED off
void off() {
  FastLED.clear();  // clear all pixel data
  FastLED.show();
}

// Simple method to call solidRainbow_Advanced with typical settings
void solidRainbow(int strips, int stripIDs[]) {
  int defaultOffset     = 30;
  int defaultBrightness = 255;
  int defaultSaturation = 255;
  int defaultDelayMs    = 20;
  solidRainbow_Advanced(strips, stripIDs, defaultOffset, defaultBrightness, defaultSaturation, defaultDelayMs);
}

// Syncronized solid rainbow across all strips, separated by offset
void solidRainbow_Advanced(int strips, int stripIDs[], int offset, int brightness, int saturation, int delayMs) {
  tick(1,0,255);
  for (int i=0;i<strips;i++) {
    for(int j=0;j<NUM_LEDS[i];j++) {
      leds[stripIDs[i]][j].setHSV(primaryTicker + offset * i, saturation, brightness);
    }
  }
  FastLED.show();
  delay(delayMs); 
}

// Timed sequence that slowly fades in solid raimbow
void alarmClock(long sleepMinutes, int wakingDisplayMode) {
  off();
  long sleepMillis = (sleepMinutes * 60 * 1000);
  delay(sleepMillis);
  displayMode = wakingDisplayMode;
};

// Runs blob displayMode with the current palette
void blobs(int totalBlobs) {
  int maxVelocity = 4;
  FastLED.clear(); 
  blobCount = totalBlobs;
  for (int i=0;i<blobCount;i++) {
    if (blob_Data[i][0] == -1 ||                  // if blob is ungenerated or
        blob_Data[i][2] + blob_Data[i][1] <= 0 || // if blob is entirely out of bounds
        blob_Data[i][2] - blob_Data[i][1] >= NUM_LEDS[blob_Data[i][0]]) {
      //blob_Data[i][0] = random(NUM_STRIPS); // random strip id
      blob_Data[i][0] = 0;
      blob_Data[i][1] = random(NUM_LEDS[blob_Data[i][0]] / 3); // random blob radius
      blob_Data[i][2] = random(NUM_LEDS[blob_Data[i][0]]); // random blob position
      blob_Data[i][3] = random(-1 * maxVelocity, maxVelocity); // random velocity
      blob_Data[i][4] = random(256); // random palette position
      while (blob_Data[i][3] == 0) { blob_Data[i][3] = random(-1 * maxVelocity, maxVelocity + 1); } // makes sure there aren't any stale blobs
    }
    Serial.println("Blob " + String(i) + ": [" + 
                  String(blob_Data[i][0]) + ", " + 
                  String(blob_Data[i][1]) + ", " + 
                  String(blob_Data[i][2]) + ", " + 
                  String(blob_Data[i][3]) + ", " + 
                  String(blob_Data[i][4]) + "]");
    for (int j=blob_Data[i][2]-blob_Data[i][1];j<=blob_Data[i][2]+blob_Data[i][1];j++) {
      if (j >= 0 && j < NUM_LEDS[blob_Data[i][0]]) {
        //leds[blob_Data[i][0]][j] = ColorFromPalette( currentPalette, blob_Data[i][4], 255, currentBlending);
        leds[blob_Data[i][0]][j].setHSV(blob_Data[i][4],255,255);
        Serial.println(String(j));
      }
    }
  }
  for (int i=blobCount;i<MAX_BLOBS;i++) { blob_Data[i][0] = -1; } // erases any extra blobs
  FastLED.show();
}

// Plots out the current palette and moves changePx pixels each iteration
void plotPalette(int strips, int stripIDs[], int changePx) {
  int defaultDelay = 40;
  tick(changePx,0,255);
  for (int i=0;i<strips;i++) {
    for (int j=0;j<NUM_LEDS[stripIDs[i]];j++) {
      leds[i][j] = ColorFromPalette(currentPalette, map((j+primaryTicker)%255,0,NUM_LEDS[stripIDs[i]],0,255), 255, currentBlending);
    }
  }
  FastLED.show();
  delay(defaultDelay);
}

// Like solidPaletteFade, but with different indexes at each end
void paletteFade(int strips, int stripIDs[], double skew, int brightness, int delayMs) {
  tick(1,0,255);
  for (int i=0;i<strips;i++) {
    for(int j=0;j<NUM_LEDS[i];j++) {
      leds[stripIDs[i]][j] = ColorFromPalette(currentPalette, primaryTicker + (int)(skew * j), brightness, currentBlending);
    }
  }
  FastLED.show();
  delay(delayMs); 
}

// Like solidRainbow, only with the current palette
void solidPaletteFade(int strips, int stripIDs[]) {
  int defaultOffset     = 30;
  int defaultBrightness = 255;
  int defaultDelayMs    = 40;
  solidPaletteFade_Advanced(strips, stripIDs, defaultOffset, defaultBrightness, defaultDelayMs);
}

// Advanced control for solidPaletteFade
void solidPaletteFade_Advanced(int strips, int stripIDs[], int offset, int brightness, int delayMs) {
  tick(1,0,255);
  for (int i=0;i<strips;i++) {
    for(int j=0;j<NUM_LEDS[i];j++) {
      leds[stripIDs[i]][j] = ColorFromPalette(currentPalette, primaryTicker + offset * i, brightness, currentBlending);
    }
  }
  FastLED.show();
  delay(delayMs); 
}

// Flickers throughout the current palette
void fireFromPalette(int strips, int stripIDs[], int brightness, int waitIters, int lagIncrement, int delayMs) {
  for (int i=0;i<strips;i++) {
    if (fireTemps[stripIDs[i]][0] == -1 || primaryTicker == 0) {
      for (int j=0;j<NUM_LEDS[stripIDs[i]];j++) {
        fireTemps[stripIDs[i]][j] = random(255);
        //realFireTemps[stripIDs[i]][j] = constrain*fireTemps[stripIDs[i]][j] + random(-64,64),0,255);
      }
    }
    tick(1,0,waitIters);
    /**
    if (primaryTicker == 0) {
      int randIndex = random(255);
      fireTemps[stripIDs[i]][randIndex] = random(255);
    }
    **/
    for (int j=0;j<NUM_LEDS[stripIDs[i]];j++) {
      if (realFireTemps[stripIDs[i]][j] < fireTemps[stripIDs[i]][j]) { realFireTemps[stripIDs[i]][j] += lagIncrement; }
      if (realFireTemps[stripIDs[i]][j] > fireTemps[stripIDs[i]][j]) { realFireTemps[stripIDs[i]][j] -= lagIncrement; }
      leds[stripIDs[i]][j] = ColorFromPalette(currentPalette, realFireTemps[stripIDs[i]][j], brightness, currentBlending);
    }
  }
  FastLED.show();
  delay(delayMs);
}

void daylight(int strips, int stripIDs[], int warmth) {
  currentPalette = warm2cool_gp;
  for (int i=0;i<strips;i++) {
    if (fireTemps[stripIDs[i]][0] == -1 || primaryTicker == 0) {
      for (int j=0;j<NUM_LEDS[stripIDs[i]];j++) {
        leds[stripIDs[i]][j] = ColorFromPalette(currentPalette, 255 - constrain(warmth,0,255), 255, currentBlending);
        //leds[stripIDs[i]][j].setRGB(255,176,240);
      }
    }
  }
  FastLED.show();
}