#include <FastLED.h>

// Pin definitions for components
const int micPin = A0;    // GPIO A0 (ADC) pin for the MAX9814 microphone
//const int ledPin = 9;     // GPIO 9 pin for the standalone LED
#define LED_PIN     5     // GPIO 5 pin for the FastLED strip
#define NUM_LEDS    100   // Number of LEDs in the FastLED strip
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];      // LED array for FastLED

#define UPDATES_PER_SECOND 100
const int noiseThreshold = 285;// Noise threshold for microphone

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
  // Initialize pins
  pinMode(ledPin, OUTPUT);

  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize FastLED
  delay(3000); // Power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {
  // Read microphone signal
  int micValue = analogRead(micPin);

  // Debugging: print microphone sensor value
  Serial.print(" | Mic: ");
  Serial.println(micValue);

  // If noise level exceeds the threshold, turn on LED and run FastLED functions
  if (micValue > noiseThreshold) {
    digitalWrite(ledPin, HIGH);  // Turn on standalone LED
    
    // Run FastLED functions when noise is detected
    ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; // Motion speed

    FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  } else {
    digitalWrite(ledPin, LOW);   // Turn off standalone LED
    FastLED.clear();             // Turn off FastLED strip when no noise
    FastLED.show();
  }

  // Small delay to avoid overloading the processor
  delay(100);
}

// Function to fill the LED strip with colors from the palette
void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

// Function to change the palette periodically
void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    if (secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
    if (secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
    if (secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
    if (secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
    if (secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
    if (secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
    if (secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
    if (secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
    if (secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
    if (secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
    if (secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes.
void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green  = CHSV(HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}

// Static color palette stored in PROGMEM
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
  CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
};
