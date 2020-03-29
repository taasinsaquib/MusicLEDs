// #define LOG_OUT 1 // use the log output function
#define OCTAVE 1
#define FHT_N  256 // set to 256 point fht

#include <FHT.h>
#include <FastLED.h>

#define LED_PIN        6

#define NUM_LEDS       2
#define LED_BRIGHTNESS 25 // Set BRIGHTNESS to about 1/5 (max = 255)

CRGB leds[NUM_LEDS];

int val = 0;

void setup(){
  Serial.begin(9600);

  // TIMSK0 = 0;    // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40;  // use adc0
  DIDR0 = 0x01;  // turn off the digital input for adc0

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
}

int b,c,d;

void loop(){

  leds[0] = CRGB::White;

  Serial.println(b);

  if (b > 10){
    leds[1] = CRGB::White;
    FastLED.setBrightness(LED_BRIGHTNESS + 50);
    FastLED.show();
    delay(500);
  }
  else {
    leds[1] = CRGB::Black;
    FastLED.show();
  }

  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
      // Serial.println(k);
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    // fht_mag_log(); // take the output of the fht
    fht_mag_octave();
    sei();

    // Serial.write(255); // send a start byte
    // Serial.write(fht_log_out, FHT_N/2); // send out the data

    // for (int i = 0; i < FHT_N/2; i++){
    //  Serial.println(fht_log_out[i]);
    // }

    // for (int i = 0; i < 8; i++){
    //  Serial.print(fht_oct_out[i]);
    //  Serial.print(" ");
    // }
    // Serial.println("*********************");
    // int a = fht_oct_out[0] - 136;
    b = fht_oct_out[1] - 115;
    c = fht_oct_out[2] - 90;
    d = fht_oct_out[3] - 75;
    // int e = fht_oct_out[4] - 70;
    Serial.println(b);
    // Serial.print(",");
    // Serial.print(c);
    // Serial.print(",");
    // Serial.print(d);
    // Serial.print(",");
    // Serial.println(fht_oct_out[4]);
  }
}