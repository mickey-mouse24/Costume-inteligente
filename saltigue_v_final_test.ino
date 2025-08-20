// Définition des broches pour les composants
const int pirPin1 = 5;    // Broche GPIO 5 pour le premier capteur PIR
const int pirPin2 = 6;    // Broche GPIO 6 pour le deuxième capteur PIR
const int micPin = A0;    // Broche GPIO A0 (ADC) pour le micro MAX9814
const int ledPin = 10;    // Broche GPIO 10 pour la LED

// Seuil pour le bruit capté par le micro
const int noiseThreshold = 1500; // Ajuster ce seuil en fonction de l'environnement

#include <FastLED.h>

#define LED_STRIP_PIN 11  // Broche GPIO 9 pour la bande LED (séparée de la LED individuelle)
#define NUM_LEDS 250 
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType currentBlending;

// Fonctions externes et palettes pour les couleurs
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
  // Initialisation des broches pour les capteurs et la LED
  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  pinMode(ledPin, OUTPUT);

  // Initialisation de la communication série pour le débogage
  Serial.begin(115200);

  // Initialisation de la bande LED FastLED
  delay(3000);  // Délai de sécurité pour la mise sous tension
  FastLED.addLeds<LED_TYPE, LED_STRIP_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Configuration initiale de la palette et du mélange
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {
  // Lecture des valeurs des capteurs PIR
  int pirState1 = digitalRead(pirPin1);
  int pirState2 = digitalRead(pirPin2);

  // Lecture du signal du micro
  int micValue = analogRead(micPin);

  // Débogage : afficher les valeurs des capteurs
  Serial.print("PIR1: ");
  Serial.print(pirState1);
  Serial.print(" | PIR2: ");
  Serial.print(pirState2);
  Serial.print(" | Mic: ");
  Serial.println(micValue);

  // Si l'un des PIR détecte un mouvement ou si le bruit dépasse le seuil
  if (pirState1 == HIGH || pirState2 == HIGH || micValue > noiseThreshold) {
    digitalWrite(ledPin, HIGH);  // Allumer la LED
  } else {
    digitalWrite(ledPin, LOW);   // Éteindre la LED
  }

  // Gestion de la bande LED avec FastLED
  ChangePalettePeriodically();
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  
  FillLEDsFromPaletteColors(startIndex);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  // Petite pause pour éviter de surcharger le processeur
  delay(100);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

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

void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green  = CHSV(HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16(
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black
  );
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
  CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
};
