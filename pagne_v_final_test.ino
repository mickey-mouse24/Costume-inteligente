#include <Adafruit_VS1053.h>
#include <SD.h>
#include <SPI.h>

#define BOUTON        12
#define VS1053_RESET  -1
#define VS1053_CS     6
#define VS1053_DCS    10
#define CARDCS        5
#define VS1053_DREQ   9

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

bool lastButtonState = false;

void setup() {
  Serial.begin(115200);
  
  Serial.println("Adafruit VS1053 FeatherWing Test");

  pinMode(BOUTON, INPUT_PULLUP);

  if (!musicPlayer.begin()) {
    Serial.println("Couldn't find VS1053, check wiring!");
    while (1);
  }
  Serial.println("VS1053 found");

  if (!SD.begin(CARDCS)) {
    Serial.println("SD failed, or not present");
    while (1);
  }
  Serial.println("SD OK!");

  musicPlayer.setVolume(10, 10);

  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
}

void loop() {
  bool currentButtonState = digitalRead(BOUTON) == LOW;

  if (currentButtonState != lastButtonState) {
    if (currentButtonState) {
      // Button is pressed
      if (!musicPlayer.playingMusic) {
        Serial.println("Starting music");
        if (musicPlayer.startPlayingFile("/TRACK001.mp3")) {
          Serial.println("Started playing");
        } else {
          Serial.println("Could not open file!");
        }
      }
    } else {
      // Button is released
      if (musicPlayer.playingMusic) {
        Serial.println("Stopping music");
        musicPlayer.stopPlaying();
      }
    }
    lastButtonState = currentButtonState;
  }

  delay(50);  // Small delay to debounce the button
}
