 #include <Adafruit_VS1053.h>
#include <SD.h>
#include <SPI.h>

#define BOUTON        12
#define LED           11
//#define VS1053_RESET  -1
#define VS1053_CS     6
#define VS1053_DCS    10
#define CARDCS        5
#define VS1053_DREQ   9
 
Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer( VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

boolean buttonWasUp = true;
boolean musicPlaying = false;

void setup() {
  Serial.begin(115200);
  
  Serial.println("Adafruit VS1053 FeatherWing Test");

  pinMode(BOUTON, INPUT_PULLUP);  // Corrected: using BOUTON instead of BUTTON
  pinMode(LED, OUTPUT);

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
  boolean buttonIsUp = digitalRead(BOUTON);  // Corrected: using BOUTON instead of BUTTON

  if (buttonWasUp && !buttonIsUp) {  // Button was released and is now pressed
    delay(10);  // Debouncing delay
    buttonIsUp = digitalRead(BOUTON);  // Check again for debounce

    if (!buttonIsUp) {
      musicPlaying = !musicPlaying;  // Toggle the musicPlaying state

      if (musicPlaying) {
        Serial.println("Starting music");
        if (musicPlayer.startPlayingFile("/TRACK001.mp3")) {
          digitalWrite(LED, HIGH);  // Turn on the LED
          Serial.println("Started playing");
        } else {
          Serial.println("Could not open file!");
        }
      } else {
        Serial.println("Stopping music");
        musicPlayer.stopPlaying();  // Stop playing music
        digitalWrite(LED, LOW);  // Turn off the LED
      }
    }
  }

  buttonWasUp = buttonIsUp;  // Update the last button state
}
