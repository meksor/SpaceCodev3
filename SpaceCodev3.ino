#include "Adafruit_WS2801.h"
#include "SPI.h"

//define clock/data pins
uint8_t dataPin  = PIN_F1;
uint8_t clockPin = PIN_F0;

Adafruit_WS2801 strip = Adafruit_WS2801(55, dataPin, clockPin);

// define number of LEDs
#define FRONTLEDAMOUNT 28
#define BACKLEDAMOUNT 22
#define TOTALLEDAMOUNT (FRONTLEDAMOUNT + BACKLEDAMOUNT)

//define front and back LEDs !!begins at 1!!
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

typedef union{
 struct{
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t __res__;
 };
 uint32_t color;
}rgb_t;

typedef union{
 struct{
  uint8_t lo;
  uint8_t hi;
 };
 uint16_t val;
}word_t;

rgb_t leds[TOTALLEDAMOUNT];
uint8_t brightness=255;
uint8_t mode=0;
uint8_t override=0;
uint8_t delay_num=3;
rgb_t alert_color;



void setup() {
 strip.begin();

 strip.show();
 Serial1.begin(19200);

}


//command-parsing magic
void parse_command(int8_t command,uint8_t a,uint8_t b,uint8_t d){
 if(command==0){
  setBrightness(a);
  mode=b;
  override=0;
  return;
 }
 if(command>=-22&&command<=28){
  rgb_t c;
  c.r=a;
  c.g=b;
  c.b=d;
  setPixel(command,c.color);
  return;
 }
 if(command==-128){
  update();
  return;
 }
 if(command==64){
  override=a;
  return;
 }
 if(command==65){
  delay_num=a;
  return;
 }
 if(command==66){
  alert_color.r=a;
  alert_color.g=b;
  alert_color.b=d;
  return;
 }
}

uint8_t anti_block=0;

//main loop -- reads commands from Serial11 and parses them
void loop() {
 if(Serial1.available()>4){
  anti_block=0;
  if(Serial1.read()=='0'){
   uint8_t command=Serial1.read();
   uint8_t a=Serial1.read();
   uint8_t b=Serial1.read();
   uint8_t c=Serial1.read();
   parse_command(command,a,b,c);
  }
  else{
   Serial1.read();
   Serial1.read();
   Serial1.read();
   Serial1.read();
  }
 }
 else if(Serial1.available()){
  anti_block++;
  if(anti_block>50){
   while(Serial1.read()!=-1);
   anti_block=0;
  }
 }
 if(override){
  alert();delay(delay_num);
 }
 else if(brightness){
  if(mode==0){rainbow();delay(delay_num);}
 }
 else delay(delay_num);


}

//sees if adressed LEDs are on the front/back
void setPixel(int8_t PixelNo, uint32_t c) {
 if(PixelNo<0)
  PixelNo=stripBackLEDs[-PixelNo-1];
 else
  PixelNo=stripFrontLEDs[PixelNo-1];
 leds[PixelNo-1].color=c;
}

//sets brightness
void setBrightness(uint8_t val){
 word_t channel;


 if(val==255)brightness=255;
 else{channel.val=val;channel.val*=channel.val;brightness=channel.hi;
 
  if(brightness==0){
  for(int i=0;i<50;i++)
  strip.setPixelColor(i,0);strip.show();
 }
 }
}

//fills all LEDs
void fill(uint32_t c){
 for(uint8_t i=0;i<50;i++)
  leds[i].color=c;
}

//update function to replace show()
void update(){
 rgb_t temp;
 word_t channel;
 if(brightness==255)
  for(uint8_t i=0;i<TOTALLEDAMOUNT;i++){
   strip.setPixelColor(i,leds[i].color);
  }
 else
  for(uint8_t i=0;i<TOTALLEDAMOUNT;i++){
   temp.color=leds[i].color;
   channel.val=temp.r;channel.val*=brightness;temp.r=channel.hi;
   channel.val=temp.g;channel.val*=brightness;temp.g=channel.hi;
   channel.val=temp.b;channel.val*=brightness;temp.b=channel.hi;
   strip.setPixelColor(i,temp.color);
  }
 strip.show();
}

//alert function
uint8_t alert_state=0;
void alert(){
 rgb_t led;
 word_t channel;
 uint8_t i=alert_state;
 uint8_t val=(i<32)?i:(63-i);
 val=(val<<3)|(val>>2);
 if(val==255){
  led.color=alert_color.color;
 }
 else{
  led.color=alert_color.color;
  channel.val=led.r;channel.val*=val;led.r=channel.hi;
  channel.val=led.g;channel.val*=val;led.g=channel.hi;
  channel.val=led.b;channel.val*=val;led.b=channel.hi;
 }
 for(uint8_t j=0;j<TOTALLEDAMOUNT;j++)
  strip.setPixelColor(j,led.color);
 strip.show();
 alert_state++;
 if(alert_state==64){
  override--;
  alert_state=0;
 }
}

//rainbow function
uint8_t rainbow_state=0;
void rainbow() {
 rainbow_state++;
 for (uint8_t i=0; i < FRONTLEDAMOUNT; i++) {
  if(i<BACKLEDAMOUNT)setPixel(-i-1, Wheel( (i + rainbow_state) % 255));
  setPixel(i+1, Wheel( (i + rainbow_state) % 255));
 }
 update();
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g - b - back to r
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
