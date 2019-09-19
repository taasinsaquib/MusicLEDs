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

#include <FastLED.h>

#define OCTAVE   1    // Group buckets into octaves
#define OCT_NORM 0    // Don't normalise octave intensities by number of bins
#define FHT_N    256  // set to 256 point fht
#include <FHT.h>

// Macros ------------------------------------------------------------------

// Pins
#define ENV_PIN   5   // [in]  Envelope output of the sound detector
#define AUDIO_PIN 6   // [in]  Audio output of the sound detector
#define LED_PIN   6   // [out] Pin to write to LED strip

#define LED_COUNT 60  // LEDs in strip
#define HALFWAY   LED_COUNT/2

#define LED_GROUP_SIZE 4

// Globals -----------------------------------------------------------------

CRGB leds[LED_COUNT];  // array to hold LED color values

int envelopeValue = 0; // store current volume reading

int numLEDs = 0;

// set RGB values
// todo - make color depend on frequency and position on strip
int r = 0;
int g = 0;
int b = 0;

int maxVol = 5;
int avgVol = 0;

//just test output without in silence, and use values from ocatve bins
int noise[] = {90, 125, 147, 143, 128, 123, 104, 89};

// setup() function -- runs once at startup --------------------------------

void setup()
{
  Serial.begin(9600); //Sets data rate for serial data transmission.

  // Init FastLED object
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(25);  // Set BRIGHTNESS to about 1/5 (max = 255)
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop()
{
  // int audioVal = analogRead(AUDIO_PIN);
  // Serial.println(audioVal, DEC);

  //Record the volume level from the sound detector
  envelopeValue = analogRead(ENV_PIN);
  // Serial.println(envelopeValue, DEC);

  if (envelopeValue > maxVol)
  {
    maxVol = envelopeValue;
  }

  avgVol = (avgVol + envelopeValue)/2;

  int currNumLEDs = map(envelopeValue, 0, maxVol, 5, HALFWAY);

  if (currNumLEDs < numLEDs)
  {
    numLEDs -= 1;
  }
  else if(currNumLEDs > numLEDs)
  {
    numLEDs += 1;
  }

  for (int i = 0 ; i < FHT_N ; i++)
  {
    int k = analogRead(AUDIO_PIN);
    k -= 0x0200; // form into a såigned int
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins
  }

  int fht_noise_adjusted[8];
  int result_octaves[8];
  for (int i = 0; i < 8; i++) {  // For each of the 6 useful octave bins
    fht_noise_adjusted[i] = abs(fht_oct_out[i] - noise[i]); // take the pink noise average level out, take the asbolute value to avoid negative numbers
    fht_noise_adjusted[i] = constrain(fht_noise_adjusted[i], 37, 125); // 37 lowpass for noise; 125 high pass doesn't go much higher than this [found by trial and error]
    result_octaves[i] = map(fht_noise_adjusted[i], 37, 125, 0, 255); // map to values 0 - 160, i.e. blue to red on colour spectrum - larger range gives more colour variability [found by trial and error]     
  }
  // Serial.print("\n");

  runFHT();

  b = 0;
  g = 0;
  r = 0;

  if (result_octaves[2] != 0)
  {
    b = result_octaves[2];
    g = 0;
    r = 0;
  }

  if (result_octaves[3] != 0)
  {
    b = result_octaves[2];
    g = result_octaves[3];
    r = 0;
  }

  if (result_octaves[4] != 0)
  {
    b = 0;
    g = result_octaves[4];
    r = 0;
  }

  if (result_octaves[5] != 0)
  {
    b = 0;
    g = result_octaves[4];
    r = result_octaves[5];
  }

  if (result_octaves[6] != 0)
  {
    b = 0;
    g = 0;
    r = result_octaves[6];
  }

  // if (result_octaves[7] != 0)
  // {
  //   b = 50;
  //   g = 50;
  //   r = 50;
  // }

  // b = (result_octaves[2] + result_octaves[3]) % 255;
  // g = (result_octaves[4] + result_octaves[5]) % 255;
  // r = (result_octaves[6] + result_octaves[7]) % 255;

  // todo - doesn't work without a print statement?
  // sendToPC(&result_octaves[3]);
  // Serial.println(fht_oct_out[0]);
  // Serial.println(fht_oct_out[1]);
  Serial.println(fht_oct_out[2]); // 35-60
  // Serial.println(fht_oct_out[3]); // 100-300
  // Serial.println(fht_oct_out[4]); // 300 - 600
  // Serial.println(fht_oct_out[5]); // 600 - 10k 
  // Serial.println(fht_oct_out[6]); // 17k - 25k
  // Serial.println(fht_oct_out[7]); // 46k

  
  // Get color corresponding to volume
  // if (0 < envelopeValue && envelopeValue < 25)
  // {
  //   FastLED.setBrightness(15);
  // }
  // else if (25 < envelopeValue && envelopeValue < 50)
  // {
  //   FastLED.setBrightness(25);
  // }
  // else if(envelopeValue < 100)
  // {
  //   FastLED.setBrightness(50);
  // }
  // else
  // {
  //   FastLED.setBrightness(75);
  // }
  

  // wholeStrip();
  // wave();
  middleWave();

  FastLED.show();
  delay(100);
}

void runFHT()
{
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  fht_mag_octave(); 
  sei();
}

void sendToPC(int* data)
{
  byte* byteData = (byte*)(data);
  Serial.write(byteData, 2);
}

// Some functions of our own for creating animated effects -----------------

// LEDs updated from the center
void middleWave()
{
  // First Side
  for(int i = HALFWAY; i < HALFWAY + numLEDs; i++) { 
    leds[i].setRGB(r, g, b);
  }
  for(int i = HALFWAY + numLEDs; i < LED_COUNT; i++) { 
      leds[i].setRGB(0, 0, 0);
  }

  // Second Side
  for(int i = HALFWAY; i > HALFWAY - numLEDs; i--) { 
    leds[i].setRGB(r, g, b);
  }
  for(int i = 0; i < HALFWAY - numLEDs; i++) { 
      leds[i].setRGB(0, 0, 0);
  }
}

void wave()
{
  for (int i = 0; i < LED_GROUP_SIZE; i++)
  {
    leds[i].setRGB(r, g, b);
  }

  for (int i = 1; i < LED_COUNT - LED_GROUP_SIZE; i++)
  {
    leds[i+LED_GROUP_SIZE] = leds[i];
    leds[i].setRGB(0, 0, 0);
  }
}

void wholeStrip()
{
  for(int i = 0; i < LED_COUNT; i++) { 
      leds[i].setRGB(r, g, b);
  }
}
