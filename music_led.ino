// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

// Macros ------------------------------------------------------------------
#define ENV_PIN   5   // [in]  Envelope output of the sound detector
#define LED_PIN   6   // [out] Pin to write to LED strip

#define LED_COUNT 60  // LEDs in strip

// Globals -----------------------------------------------------------------
CRGB leds[LED_COUNT];  // array to hold LED color values

int envelopeValue = 0; // store current volume reading

// set RGB depending on volume
int r = 0;
int g = 0;
int b = 0;

// setup() function -- runs once at startup --------------------------------

void setup() {
  
  Serial.begin(9600); //Sets data rate for serial data transmission.

  // Init FastLED object
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {

  //Record the volume level from the sound detector
  envelopeValue = analogRead(ENV_PIN);
  Serial.println(envelopeValue, DEC);

  if (envelopeValue < 50)
  {
    r = 0;
    g = 0;
    b = envelopeValue % 255;
  }
  else if(envelopeValue < 100)
  {
    r = 0;
    g = envelopeValue % 255;
    b = envelopeValue % 255;
  }
  else
  {
    r = envelopeValue % 255;
    g = envelopeValue % 255;
    b = 0;
  }

  for(int i = 0; i < LED_COUNT; i++) { 
      leds[i].setRGB(r, g, b);
  }
  
  FastLED.show();
  delay(100);
}

// Some functions of our own for creating animated effects -----------------
