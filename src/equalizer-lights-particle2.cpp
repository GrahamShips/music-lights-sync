/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/graha/code/equalizer-lights-particle2/src/equalizer-lights-particle2.ino"
/*
 * Project equalizer-lights-particle
 * Description:
 * Author:
 * Date:
 */

//to generate new token, use postman -> particle environment -> generate access token, and use particle:particle as the auth header username, and fill in user and pass in the body. Save that token as env variable
//now use function
#include <neopixel.h>
#include <Adafruit_SSD1306_RK.h>
#include <Adafruit_GFX_RK.h>
// #include <Adafruit_GFX.h>
//#include <iostream>
//#include <string>
//using namespace std;

int turnOnLED(String args);
int updateConfigVars(String args);
void setup();
void loop();
#line 18 "c:/Users/graha/code/equalizer-lights-particle2/src/equalizer-lights-particle2.ino"
#define numberLEDs 130
#define maxVolume 2000
#define minVolume 300
#define oledBarWidth 16
// #define LEDData1 D2
// #define LEDData2 D3
// #define LEDData3 D4
// #define LEDData4 D5
// #define LEDData5 D6
// #define LEDData6 D8
// #define LEDData7 A3
#define LEDData1 D8
#define LEDData2 D6
#define LEDData3 D5
#define LEDData4 D4
#define LEDData5 D3
#define LEDData6 D2
#define LEDData7 A3
//#define testShift D5
#define LEDType WS2812B
//#define light D7
#define msgReset A1
#define msgStrobe A2
#define msgRead A0
#define middleLED = 65

String config = "5,5,5,5,5,5,5," //lower bounds 0-13
                "50,50,50,50,50,50,50," //upper bounds 14-27
                "254,1,1," //RGB 28-36
                "50," //brightness 37-39
                "1,1"
;

// std::string config2 = "15,15,5,15,15,15,5," //lower bounds 0-13
//                 "75,75,9,75,75,75,99," //upper bounds 14-27
//                 "122,55,12," //RGB 28-36
//                 "50" //brightness 37-39
// ;
float strip1LowerBound;
float strip2LowerBound;
float strip3LowerBound;
float strip4LowerBound;
float strip5LowerBound;
float strip6LowerBound;
float strip7LowerBound;
float strip1UpperBound;
float strip2UpperBound;
float strip3UpperBound;
float strip4UpperBound;
float strip5UpperBound;
float strip6UpperBound;
float strip7UpperBound;
int red;
int green;
int blue;
int brightness;
int colorMode;
int shapeMode;
int configArray[] = {15,15,15,15,15,15,15, //0-6   lower bounds
                    75,75,75,75,75,75,75,  //7-13  upper bounds
                    255,1,1,               //14-16 RGB
                    50,                    //17    Brightness
                    1,1};                  //18,19 Color mode, shape mode


//int LEDstate1(String args);
//String LEDstate = "LOW";
int loopNum = 0;

Adafruit_NeoPixel strip1(numberLEDs, LEDData1, LEDType);
Adafruit_NeoPixel strip2(numberLEDs, LEDData2, LEDType);
Adafruit_NeoPixel strip3(numberLEDs, LEDData3, LEDType);
Adafruit_NeoPixel strip4(numberLEDs, LEDData4, LEDType);
Adafruit_NeoPixel strip5(numberLEDs, LEDData5, LEDType);
Adafruit_NeoPixel strip6(numberLEDs, LEDData6, LEDType);
Adafruit_NeoPixel strip7(numberLEDs, LEDData7, LEDType);

//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_SSD1306 display(128,32, &Wire, -1);

int turnOnLED(String args){
  digitalWrite(D7,args.toInt());
  if (digitalRead(D7)==HIGH){
    return 1;
  }
  else if (digitalRead(D7)==LOW){
    return 0;
  }
  else{
    return 0;
  }
}

int updateConfigVars(String args){
  const String config = args;
  Serial.println(config);

  int configVar = 0;
  int tempVar = 0;
  for (int x = 0; x<config.length(); x++){
    //Serial.println(config.charAt(x));
    //String c = String(config.charAt(x));
    //Serial.println(config.charAt(x) + ' ' + tempVar);
    if (config.charAt(x)==','){
      configArray[configVar]=tempVar;
      //Serial.println(tempVar);
      tempVar=0;
      configVar++;
    }else if (x==config.length()-1){
      tempVar=(tempVar*10)+String(config.charAt(x)).toInt();
      configArray[configVar]=tempVar;
      //Serial.println(tempVar);
      tempVar=0;
      configVar++;
    }else{
      tempVar=(tempVar*10)+String(config.charAt(x)).toInt();
    }
  }
  //Serial.println("config: " + configArray[0] + ', ' + configArray[1] + ', ' + configArray[2] + ', ' + configArray[3] + ', ' + configArray[4] + ', ' + configArray[5] + ', ' + configArray[6] + ', ' + configArray[7] + ', ' + configArray[8] + ', ' + configArray[9] + ', ' + configArray[10] + ', ' + configArray[11] + ', ' + configArray[12] + ', ' + configArray[13] + ', ' + configArray[14] + ', ' + configArray[15] + ', ' + configArray[16] + ', ' + configArray[17]);
  strip1LowerBound = configArray[0];
  strip2LowerBound = configArray[1];
  strip3LowerBound = configArray[2];
  strip4LowerBound = configArray[3];
  strip5LowerBound = configArray[4];
  strip6LowerBound = configArray[5];
  strip7LowerBound = configArray[6];
  strip1UpperBound = configArray[7];
  strip2UpperBound = configArray[8];
  strip3UpperBound = configArray[9];
  strip4UpperBound = configArray[10];
  strip5UpperBound = configArray[11];
  strip6UpperBound = configArray[12];
  strip7UpperBound = configArray[13];
  red = configArray[14];
  green = configArray[15];
  blue = configArray[16];
  brightness = configArray[17];
  colorMode = configArray[18];
  shapeMode = configArray[19];
  
  return 1;
}


// setup() runs once, when the device is first turned on.
void setup() {
pinMode(LEDData1,OUTPUT);
pinMode(LEDData2,OUTPUT);
pinMode(LEDData3,OUTPUT);
pinMode(LEDData4,OUTPUT);
pinMode(LEDData5,OUTPUT);
pinMode(LEDData6,OUTPUT);
pinMode(LEDData7,OUTPUT);

pinMode(msgReset,OUTPUT);
pinMode(msgStrobe,OUTPUT);

updateConfigVars(config);
Serial.begin(9600);

Particle.function("LEDState1", turnOnLED); // change second argument to be turnOnLED func
Particle.function("UpdateConfig", updateConfigVars); // change second argument to be turnOnLED func

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
}else{
  Serial.println("screen initialized successfully");
}
delay(500); // Pause for 2 seconds

  // Clear the buffer
display.clearDisplay();

  // Draw a single pixel in white
display.drawPixel(10, 10, 1);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
display.display();

strip1.begin();
strip2.begin();
strip3.begin();
strip4.begin();
strip5.begin();
strip6.begin();
strip7.begin();

strip1.show();
strip2.show();
strip3.show();
strip4.show();
strip5.show();
strip6.show();
strip7.show();

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.


loopNum ++;
if(loopNum==50){
  //digitalWrite(D3,HIGH);
  Serial.println("high");
}
if(loopNum==100){
  //digitalWrite(D3,LOW);
  loopNum =0;
  Serial.println("low");
}

digitalWrite(msgReset, HIGH);
delay(1);
digitalWrite(msgReset, LOW);
float lightValue1 = 0;
float lightValue2 = 0;
float lightValue3 = 0;
float lightValue4 = 0;
float lightValue5 = 0;
float lightValue6 = 0;
float lightValue7 = 0;

float percentFill = 0;
display.clearDisplay();
for (int x = 0; x<7; x++){
  digitalWrite(msgStrobe, LOW);
  delayMicroseconds(35);
  int currentVal = analogRead(msgRead);
  if(x==0){
    lightValue1=currentVal;
  }
  if(x==1){
    lightValue2=currentVal;
  }
  if(x==2){
    lightValue3=currentVal;
  }
  if(x==3){
    lightValue4=currentVal;
  }
  if(x==4){
    lightValue5=currentVal;
  }
  if(x==5){
    lightValue6=currentVal;
  }
  if(x==6){
    lightValue7=currentVal;
    lightValue2=(lightValue1+lightValue3)/2;
  }
  
  //calculate percentFill
  percentFill = ((float)currentVal - (float)minVolume)/((float)maxVolume-(float)minVolume);
  //update OLED buffer
  for (int i = 0; i<oledBarWidth-2; i++){
    display.drawFastVLine((127-i)-(oledBarWidth*x),31-(31*percentFill),31*percentFill,WHITE);
  }


  digitalWrite(msgStrobe, HIGH);
  delayMicroseconds(35);
}

//update oled and LEDs from buffers
//display.clearDisplay();

display.display();

//Serial.println();
strip1.clear();
strip2.clear();
strip3.clear();
strip4.clear();
strip5.clear();
strip6.clear();
strip7.clear();

float strip1LowerBoundScaled = (strip1LowerBound*4095)/100;
float strip2LowerBoundScaled = (strip2LowerBound*4095)/100;
float strip3LowerBoundScaled = (strip3LowerBound*4095)/100;
float strip4LowerBoundScaled = (strip4LowerBound*4095)/100;
float strip5LowerBoundScaled = (strip5LowerBound*4095)/100;
float strip6LowerBoundScaled = (strip6LowerBound*4095)/100;
float strip7LowerBoundScaled = (strip7LowerBound*4095)/100;

float strip1UpperBoundScaled = (strip1UpperBound*4095)/100;
float strip2UpperBoundScaled = (strip2UpperBound*4095)/100;
float strip3UpperBoundScaled = (strip3UpperBound*4095)/100;
float strip4UpperBoundScaled = (strip4UpperBound*4095)/100;
float strip5UpperBoundScaled = (strip5UpperBound*4095)/100;
float strip6UpperBoundScaled = (strip6UpperBound*4095)/100;
float strip7UpperBoundScaled = (strip7UpperBound*4095)/100;

float strip1PercentFill = 100*(lightValue1-strip1LowerBoundScaled)/(strip1UpperBoundScaled-strip1LowerBoundScaled);
float strip2PercentFill = 100*(lightValue2-strip2LowerBoundScaled)/(strip2UpperBoundScaled-strip2LowerBoundScaled);
float strip3PercentFill = 100*(lightValue3-strip3LowerBoundScaled)/(strip3UpperBoundScaled-strip3LowerBoundScaled);
float strip4PercentFill = 100*(lightValue4-strip4LowerBoundScaled)/(strip4UpperBoundScaled-strip4LowerBoundScaled);
float strip5PercentFill = 100*(lightValue5-strip5LowerBoundScaled)/(strip5UpperBoundScaled-strip5LowerBoundScaled);
float strip6PercentFill = 100*(lightValue6-strip6LowerBoundScaled)/(strip6UpperBoundScaled-strip6LowerBoundScaled);
float strip7PercentFill = 100*(lightValue7-strip7LowerBoundScaled)/(strip7UpperBoundScaled-strip7LowerBoundScaled);


// float strip3PercentFill = (100*(lightValue1-strip3LowerBound))/((4095*strip3UpperBound)-(4095*strip3LowerBound));
//float strip7PercentFill = (100*(lightValue7-strip7LowerBound))/((4095*strip7UpperBound)-(4095*strip7LowerBound));

int j;

if (shapeMode==2){
  for(j=0; j<numberLEDs; j++){
    if(strip1PercentFill>((100*j)/numberLEDs)){
      strip1.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip2PercentFill>((100*j)/numberLEDs)){
      strip2.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip3PercentFill>((100*j)/numberLEDs)){
      strip3.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip4PercentFill>((100*j)/numberLEDs)){
      strip4.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip5PercentFill>((100*j)/numberLEDs)){
      strip5.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip6PercentFill>((100*j)/numberLEDs)){
      strip6.setColorDimmed(j, red, green, blue, brightness);
    }
    if(strip7PercentFill>((100*j)/numberLEDs)){
      strip7.setColorDimmed(j, red, green, blue, brightness);
    }
  }
}else if (shapeMode==1){
  for(j=0; j<numberLEDs; j++){
    if(abs(j-55)<(numberLEDs*(strip1PercentFill/2)/100)){
      strip1.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip2PercentFill/2)/100)){
      strip2.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip3PercentFill/2)/100)){
      strip3.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip4PercentFill/2)/100)){
      strip4.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip5PercentFill/2)/100)){
      strip5.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip6PercentFill/2)/100)){
      strip6.setColorDimmed(j, red, green, blue, brightness);
    }
    if(abs(j-55)<(numberLEDs*(strip7PercentFill/2)/100)){
      strip7.setColorDimmed(j, red, green, blue, brightness);
    }    
}
}

strip1.show();
strip2.show();
strip3.show();
strip4.show();
strip5.show();
strip6.show();
strip7.show();

}

