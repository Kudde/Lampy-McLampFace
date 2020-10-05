#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

// Functions
uint32_t Wheel(byte WheelPos);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
void colorAll(uint32_t color);
void turnOff();
void setBrightness(uint32_t level);
void loadMail();
void decodeMail();
void clearMail();

// Lights
int pixels          = 4 * 14;
int state           = 0;
uint32_t red        = 255;
uint32_t green      = 64;
uint32_t blue       = 0;
uint32_t light      = 100;


// Bluetooth
char mailbox[50];
int mb_index = 0;
bool message_flag = false;

// States
const int STATE_OFF     = 0;
const int STATE_GREEN   = 1;
const int STATE_BLUE    = 2;
const int STATE_RED     = 3;
const int STATE_RAIN    = 4;
const int STATE_CHASE   = 5;
const int STATE_PULSE   = 6;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, PIN, NEO_GRB + NEO_KHZ800);

// ------------------------------- MAIN


void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  state = STATE_OFF;
  colorAll(strip.Color(red, green, blue));
  setBrightness(light);


}

void loop() {

  loadMail();


}


// ------------------------------- FUN

void loadMail() {
  if (Serial.available()) {

        char c = Serial.read();
        mailbox[mb_index++] = c;

        if (mailbox[mb_index-1] == '!') //end of message
          decodeMail();

      }

}

// 0 - 255
// red:green:blue:fade:!
void decodeMail() {

  Serial.println(mailbox);
  char delimiter = ':';
  char end = '!';

  String values[4];
  int val_index = 0;
  String v = "";

  char c;
  int mail_index = 0;

  c = mailbox[0];
  while (c != end) {

    // Add value to list and move to next
    if (c == delimiter) {
      values[val_index] = v;
      v = "";
      val_index++;

    } else
      v = v + c;

    mail_index++;
    c = mailbox[mail_index];

  }

  red     = values[0].toInt();
  green   = values[1].toInt();
  blue    = values[2].toInt();
  light   = values[3].toInt();

  colorAll(strip.Color(red, green, blue));
  setBrightness(light);


  clearMail();


}

void clearMail() {
  memset(mailbox, 0, sizeof(mailbox));
  mb_index = 0;
  mailbox[0] = 0;
}

// 0 - 255
void setBrightness(uint32_t level) {
  strip.setBrightness(level);
}

void turnOff() {
  for (uint16_t i = 0 ; i < strip.numPixels() ; i++)
    strip.setPixelColor(i, 0);

}

void colorAll(uint32_t color) {
  for (uint16_t i = 0 ; i < strip.numPixels() ; i++)
    strip.setPixelColor(i, color);

  strip.show();


}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0 ; i < strip.numPixels() ; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for( j = 0 ; j < 256 ; j++) {
    for( i = 0 ; i < strip.numPixels() ; i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j = 0 ; j < 256 * 5 ; j++) { // 5 cycles of all colors on wheel
    for(i = 0 ; i < strip.numPixels() ; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0 ; j < 10 ; j++) {  //do 10 cycles of chasing
    for (int q = 0 ; q < 3 ; q++) {
      for (uint16_t i = 0 ; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;

  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }

  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }

  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
