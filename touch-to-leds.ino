// compiler error handling
#include "Compiler_Errors.h"
#include "LedStrip.cpp"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4


// LED definitions
#define LED_PIN       8
#define NO_LEDS      50

// Animation Settings
#define UP_TO        30
#define SENSOR_INDEX 10
LedStrip leds = LedStrip(LED_PIN, NO_LEDS);
LedStrip leds2 = LedStrip(LED_PIN, NO_LEDS);

const int      BNS        = 30; // BRIGHTNESS
const uint32_t WHITE      = leds.Color(BNS, BNS, BNS);
const uint32_t GREEN      = leds.Color(0  , 100, 0);
const uint32_t BLACK      = leds.Color(0  , 0  , 0);

const int      easeOutDur = 2000;
const int      pauseDur   = 1000;
const int      easeInDur  = 3000;

bool shouldEaseOutBottom;
bool shouldEaseOutTop;
bool shouldEaseIn;

void setup(){  
  Serial.begin(57600);
//  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);

  leds.begin();
  leds.clear();
  leds.show();
  leds.colorRange(1,UP_TO, WHITE);
   
}

void loop(){
  unsigned long currentMillis = millis();
  
  readTouchInputs();
  
  if(shouldEaseOutBottom){
    shouldEaseOutBottom = leds.easeInRangeMillis(currentMillis, 1,SENSOR_INDEX, easeOutDur,BLACK);
    if (!shouldEaseOutBottom){
      Serial.println("Ease Out Bottom Ended");
      onEaseBottomEnd();
    }
  }
  if(shouldEaseOutTop){
    shouldEaseOutTop = leds2.easeInRangeMillis(currentMillis, UP_TO,SENSOR_INDEX+1, easeOutDur,BLACK);
    if (!shouldEaseOutTop){
      Serial.println("Ease Out Top Ended");
      onEaseTopEnd();
    }
  }
  if(shouldEaseIn){
    shouldEaseIn = leds.easeInRangeMillis(currentMillis, 1,UP_TO, easeInDur,WHITE);
    if (!shouldEaseIn){
      Serial.println("Ease In Ended");
      onEaseInEnd();
    }
  }
}

void onEaseBottomEnd(){
  delay(pauseDur);
  shouldEaseIn = true;
}

void onEaseTopEnd(){
  // pass
}

void onEaseInEnd(){
  leds.colorRange(1,UP_TO, WHITE);
}

void touch(){
  Serial.println("Touch");
  shouldEaseOutBottom = true;
  shouldEaseOutTop    = true;
}

void untouch(){
  // pass
}

void readTouchInputs(){
  if(MPR121.touchStatusChanged()){
    
    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches
    
    if(MPR121.getNumTouches()<=1){
      for (int i=0; i < 12; i++){  // Check which electrodes were pressed
        if(MPR121.isNewTouch(i)){
            //pin i was just touched
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
            if(i == 0){ // respond to touchPin 0
              touch();
            }
        }else{
          if(MPR121.isNewRelease(i)){
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
            if(i == 0){ // respond to touchPin 0
              untouch();
            }
         } 
        }
      }
    }
  }
}
