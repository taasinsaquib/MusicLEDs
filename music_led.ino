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
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN   6   // Which pin on the Arduino is connected to the NeoPixels?

#define LED_COUNT 60  // How many NeoPixels are attached to the Arduino?

#define ENV_PIN   5   // Pin for the envelope of the sound detector

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

int envelopeValue = 0;

// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  
  Serial.begin(9600); //Sets data rate for serial data transmission.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  envelopeValue = analogRead(ENV_PIN);       //Record the volume level from the sound detector
  Serial.println(envelopeValue, DEC);

  uint32_t color = strip.Color(0,   0,   0);
  uint32_t red = strip.Color(255, 0, 0);

  if (envelopeValue < 50)
  {
    color = strip.Color(0,   0,   envelopeValue % 255);
  }
  else if(envelopeValue < 100)
  {
    color = strip.Color(0,   envelopeValue % 255,   envelopeValue % 255);
  }
  else
  {
    color = strip.Color(envelopeValue % 255,   envelopeValue % 255, 0);
  }

  strip.setPixelColor(0, color);         //  Set pixel's color (in RAM)

  // For each pixel in strip...
  for(int i=1; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i-1)); // for wave effect
    // strip.setPixelColor(i, color);                // light up all pixels
  }

  //  Update strip
  strip.show();                          
  delay(50);
}

void firstTry()
{
  envelopeValue = analogRead(ENV_PIN);       //Record the volume level from the sound detector
  Serial.println(envelopeValue, DEC);
  
  uint32_t none = strip.Color(0,   0,   envelopeValue % 255);
  strip.setPixelColor(0, none);         //  Set pixel's color (in RAM)

  // For each pixel in strip...
  for(int i=1; i<strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.getPixelColor(i-1));       
    // strip.setPixelColor(i, strip.Color(0,   0,   envelopeValue % 255));  // light up all pixels
  }

  strip.show();
  delay(50);
}

// Some functions of our own for creating animated effects -----------------
