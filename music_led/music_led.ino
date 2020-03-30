// #define LOG_OUT 1 // use the log output function
#define OCTAVE 1    // use oct_function
#define FHT_N  256  // set to 256 point fht

#include <FHT.h>
#include <FastLED.h>

#define LED_PIN        6

#define NUM_LEDS       2
#define LED_BRIGHTNESS 25 // Set BRIGHTNESS to about 1/5 (max = 255)

#define DC_OFFSET 512

CRGB leds[NUM_LEDS];

void getSamples(){
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;

      /*
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
      */
      fht_input[i] = ADC - DC_OFFSET;
    }
    sei();
}

void getFHT(){
  getSamples();
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  // fht_mag_log(); // take the output of the fht
  fht_mag_octave();
    // int a = fht_oct_out[0] - 136;
    // int b1 = fht_oct_out[1] - 115;
    // c = fht_oct_out[2] - 90;
    // d = fht_oct_out[3] - 75;
    // int e = fht_oct_out[4] - 70;
    // Serial.println(b1);
    // Serial.print(",");
    // Serial.print(c);
    // Serial.print(",");
    // Serial.print(d);
    // Serial.print(",");
    // Serial.println(fht_oct_out[4]);
}

int val = 0;

void setup(){
  Serial.begin(115200);
  delay(1000);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);

  /*
  // TIMSK0 = 0;    // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40;  // use adc0
  DIDR0 = 0x01;  // turn off the digital input for adc0
  */

// Setup the ADC for polled 10 bit sampling on analog pin 5 at 19.2kHz.
  cli();                                  // Disable interrupts.
  ADCSRA = 0;                             // Clear this register.
  ADCSRB = 0;                             // Ditto.
  ADMUX = 0;                              // Ditto.
  ADMUX = 0x40;                           // Set A5 analog input pin.
  ADMUX |= (0 << REFS0);                  // Set reference voltage  (analog reference(external), or using 3.3V microphone on 5V Arduino.
                                          // Set that to 1 if using 5V microphone or 3.3V Arduino.
//  ADMUX |= (1 << ADLAR);                  // Left justify to get 8 bits of data.                                          
  ADMUX |= (0 << ADLAR);                  // Right justify to get full 10 A/D bits.

//  ADCSRA |= bit (ADPS0) | bit (ADPS2);                //  32 scaling or 38.5 KHz sampling
//  ADCSRA |= bit (ADPS1) | bit (ADPS2);                //  Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // 128 prescaler with 9.6 KHz sampling


  ADCSRA |= (1 << ADATE);                 // Enable auto trigger.
//  ADCSRA |= (1 << ADIE);                  // Enable interrupts when measurement complete (if using ISR method). Sorry, we're using polling here.
  ADCSRA |= (1 << ADEN);                  // Enable ADC.
  ADCSRA |= (1 << ADSC);                  // Start ADC measurements.
  sei();                                  // Re-enable interrupts.

  leds[0] = CRGB::White;
}

int a, b,c,d;

void loop(){

  EVERY_N_MILLISECONDS(10){
    // oct
    a = fht_oct_out[0] - 50;
    b = fht_oct_out[1] - 60;
    c = fht_oct_out[2] - 22;

    // log
    // [130, 120, 75, 50]
    // a = fht_log_out[4];
    // b = fht_log_out[5];
    // c = fht_log_out[6];
    // d = fht_log_out[7];

    // a = abs(a);
    // b = abs(b);
    // c = abs(c);

    // Serial.print(a);
    // Serial.print(",");
    // Serial.print(b);
    // Serial.print(",");
    Serial.println(b);

    
    if (abs(b) > 35){
      leds[1] = CRGB::White;
      FastLED.setBrightness(LED_BRIGHTNESS + 100);
      FastLED.show();
      delay(5);
    }
    else {
      leds[1] = CRGB::Black;
      FastLED.setBrightness(LED_BRIGHTNESS);
      FastLED.show();
    }
  }

  
  getFHT();
}