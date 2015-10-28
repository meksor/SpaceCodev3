#include "Adafruit_WS2801.h"

uint8_t dataPin  = PIN_F1;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = PIN_F0;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(55, dataPin, clockPin);

typedef union{
 struct{
  uint8_t b;
  uint8_t g;
  uint8_t r; 
 };
 uint32_t color;
}rgb_t;

rgb_t leds[50];
uint8_t brightness=255;
uint8_t state;
uint8_t mode=0;

void setup() {
 strip.begin();

 strip.show();
 Serial.begin(19200);
}


void loop() {
 IndependentRainbow(5);
}

const int stripFrontLEDs[28] = {
 22,23,24,25,19,20,27,28,
 30,31,33,34,43,42,40,39,
 37,36,49,50,13,12, 5, 4,
  2, 1, 8, 7
};

const int stripBackLEDs[22] = {
  3, 6, 9,10,11,14,15,16,
 17,18,21,26,29,32,35,38,
 41,44,45,46,47,48,
};

void setPixel(int8_t PixelNo, uint32_t c) {
 if(PixelNo<0)
  PixelNo=stripBackLEDs[-PixelNo-1];
 else
  PixelNo=stripFrontLEDs[PixelNo-1];
 leds[PixelNo].color=c;
}

void setBrightness(uint8_t val){
 brightness=val;
}

void update(){
 rgb_t temp;
 if(brightness==255)
  for(uint8_t i=0;i<50;i++){
   strip.setPixelColor(i,leds[i].color);
  }
 else
  for(uint8_t i=0;i<50;i++){
   temp.color=leds[i].color;
   temp.r*=brightness;temp.r>>=8;
   temp.g*=brightness;temp.g>>=8;
   temp.b*=brightness;temp.b>>=8;
   strip.setPixelColor(i,temp.color);
  }
 strip.show();
}


void IndependentRainbow(uint8_t wait) {
 int i, j;
 for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
  for (i=0; i < 28; i++) {
   setPixel(i+1, Wheel( (i + j) % 255));
  }
  for (i=0; i < 22; i++) {
   setPixel(-i-1, Wheel( (i + j) % 255));
  }
  update();
  delay(wait);
 }
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos){
 rgb_t temp;
 if(WheelPos<85){
  temp.r=WheelPos*3;
  temp.g=255-temp.r;
  temp.b=0;
  return temp.color;
 }
 if(WheelPos<170){
  WheelPos-=85;
  temp.b=WheelPos*3;
  temp.r=255-temp.b;
  temp.g=0;
  return temp.color;
 }
 WheelPos-=170;
 temp.g=WheelPos*3;
 temp.b=255-temp.g;
 temp.r=0;
 return temp.color;
}
