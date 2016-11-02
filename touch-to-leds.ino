// compiler error handling
#include "Compiler_Errors.h"
#include "LedStrip.cpp"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// touch behaviour definitions
#define firstPin 0
#define lastPin 11

// LED definitions
#define LED_PIN       6
#define NO_LEDS      30
#define UP_TO        25
#define SENSOR_INDEX 15
LedStrip leds = LedStrip(LED_PIN, NO_LEDS);
const int      BNS        = 30; // BRIGHTNESS
const uint32_t WHITE      = leds.Color(BNS, BNS, BNS);
const uint32_t GREEN      = leds.Color(0  , 100, 0);
const uint32_t BLACK      = leds.Color(0  , 0  , 0);

// LED pins
// maps electrode 0 to digital 0, electrode 2 to digital 1, electrode 3 to digital 10 and so on...
// A0..A5 are the analogue input pins, used as digital outputs in this example
const int ledPins[12] = {0, 1, 10, 11, 12, 13, A0, A1, A2, A3, A4, A5};

void setup(){  
  Serial.begin(57600);
   
  //while (!Serial) ; {} //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Touch MP3 player");

//  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);

  for(int i=firstPin; i<=lastPin; i++){
    pinMode(ledPins[i], OUTPUT); 
    digitalWrite(ledPins[i], LOW);
  }

  leds.begin();
  leds.clear();
  leds.show();
  leds.colorRange(1,UP_TO, WHITE);
   
}

void loop(){
  readTouchInputs();
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
              leds.easeInRange(1,SENSOR_INDEX, 500,BLACK);
              leds.easeInRange(UP_TO+1,SENSOR_INDEX+2, 500,BLACK);
              leds.setPixelColor(SENSOR_INDEX, GREEN);
              delay(3000);
              leds.easeInRange(1,UP_TO, 1000,WHITE);
            }
        }else{
          if(MPR121.isNewRelease(i)){
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
            if(i == 0){ // respond to touchPin 0
//              leds.easeInRange(1,UP_TO, 1000,WHITE);
            }
         } 
        }
      }
    }
  }
}
