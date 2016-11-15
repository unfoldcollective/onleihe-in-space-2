// compiler error handling
#include "Compiler_Errors.h"
#include "LedStrip.cpp"
#include "Adafruit_Thermal.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// Global animation vars
const int easeOutDur = 2000;
const int pauseDur   = 5000;
const int easeInDur  = 3000;

bool shouldEaseOutBottom;
bool shouldEaseOutTop;
bool shouldEaseIn;
bool shouldEaseToPrinter;
// Animation Settings
#define UP_TO          25
#define SENSOR_INDEX   10
#define PRINTER_INDEX1 68

// LED Strip 1: the interactive strip
#define LED_PIN1       8
#define NO_LEDS1      78
#define UP_TO1        77
#define SENSOR_INDEX1 54
LedStrip leds1a = LedStrip(LED_PIN1, NO_LEDS1);
LedStrip leds1b = LedStrip(LED_PIN1, NO_LEDS1);

// LED Strip 2
#define LED_PIN2      9
#define NO_LEDS2     60
#define UP_TO2       60
LedStrip leds2 = LedStrip(LED_PIN2, NO_LEDS2);

// LED Strip 3
#define LED_PIN3     10
#define NO_LEDS3     66
#define UP_TO3       66
LedStrip leds3 = LedStrip(LED_PIN3, NO_LEDS3);

// LED Strip 4
#define LED_PIN4     11
#define NO_LEDS4     30
#define UP_TO4       30
LedStrip leds4 = LedStrip(LED_PIN4, NO_LEDS4);

// LED Strip 5
#define LED_PIN5     12
#define NO_LEDS5     NO_LEDS1
LedStrip leds5 = LedStrip(LED_PIN5, NO_LEDS5);

// Global colors
const int      BNS        = 30; // BRIGHTNESS
const uint32_t WHITE      = leds1a.Color(BNS, BNS, BNS);
const uint32_t GREEN      = leds1a.Color(  0, 100,   0);
const uint32_t BLUE       = leds1a.Color(  0,   0, 100);
const uint32_t RED        = leds1a.Color(100,   0,   0);
const uint32_t BLACK      = leds1a.Color(  0,   0,   0);
// Goethe identity colors
const uint32_t GOETHE     = leds1a.Color(160, 200,  20);
const uint32_t GPURPLE    = leds1a.Color(130,   5,  95);
const uint32_t GGREEN     = leds1a.Color( 55,  65,   5);
const uint32_t GORANGE    = leds1a.Color(235, 100,   0);

const uint32_t COLOR1     = GPURPLE;
const uint32_t COLOR2     = GGREEN;
const uint32_t COLOR3     = GORANGE;
const uint32_t COLOR4     = GOETHE;

////////////////////
// Termal Printer //
////////////////////

// Here's the new syntax when using SoftwareSerial (e.g. Arduino Uno) ----
// If using hardware serial instead, comment out or remove these lines:

#include "SoftwareSerial.h"
#define TX_PIN 5 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 6 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
// Then see setup() function regarding serial & printer begin() calls.

// Here's the syntax for hardware serial (e.g. Arduino Due) --------------
// Un-comment the following line if using hardware serial:

//Adafruit_Thermal printer(&Serial1);      // Or Serial2, Serial3, etc.

void setup(){
  Serial.begin(57600);
//  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);

  leds1a.begin();
  leds1a.clear();
  leds1a.show();
  leds1a.colorRange(1,UP_TO1, COLOR1);

  leds2.begin();
  leds2.clear();
  leds2.show();
  leds2.colorRange(1,UP_TO2, COLOR2);

  leds3.begin();
  leds3.clear();
  leds3.show();
  leds3.colorRange(1,UP_TO3, COLOR3);
  
  leds4.begin();
  leds4.clear();
  leds4.show();
  leds4.colorRange(1,UP_TO4, COLOR4);

  leds5.begin();
  leds5.clear();
  leds5.show();
  leds5.colorRange(1,UP_TO1, COLOR1);

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
}

void loop(){
  unsigned long currentMillis = millis();
  
  readTouchInputs();
  
  if(shouldEaseOutBottom){
    shouldEaseOutBottom = leds1a.easeInRangeMillis(currentMillis, 1,SENSOR_INDEX1, easeOutDur,BLACK);
    if (!shouldEaseOutBottom){
      Serial.println("Ease Out Bottom Ended");
      onEaseBottomEnd();
    }
  }
  if(shouldEaseOutTop){
    shouldEaseOutTop = leds1b.easeInRangeMillis(currentMillis, UP_TO1,SENSOR_INDEX1+1, easeOutDur,BLACK);
    if (!shouldEaseOutTop){
      Serial.println("Ease Out Top Ended");
      onEaseTopEnd();
    }
  }
  if(shouldEaseIn){
    shouldEaseIn = leds1a.easeInRangeMillis(currentMillis, 1,UP_TO1, easeInDur,COLOR1);
    if (!shouldEaseIn){
      Serial.println("Ease In Ended");
      onEaseInEnd();
    }
  }
  if(shouldEaseToPrinter){
    shouldEaseToPrinter = leds1a.easeInRangeMillis(currentMillis, SENSOR_INDEX1,PRINTER_INDEX1, easeInDur,COLOR1);
    if (!shouldEaseToPrinter){
      Serial.println("Ease To Printer Ended");
      onEaseToPrinterEnd();
    }
  }
}

void onEaseToPrinterEnd () {
  leds1a.colorRange(SENSOR_INDEX1,PRINTER_INDEX1, COLOR1);
  printQuote();
  shouldEaseIn = true;
}

void onEaseBottomEnd(){
//  delay(pauseDur);
  shouldEaseToPrinter = true;
}

void onEaseTopEnd(){
  // pass
}

void onEaseInEnd(){
  leds1a.colorRange(1,UP_TO1, COLOR1);
}

void touch(){
  Serial.println("Touch");
  shouldEaseOutBottom = true;
  shouldEaseOutTop    = true;
}

void untouch(){
  // pass
}

void printQuote() {
  printer.println();
  
  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
  printer.justify('C');
  printer.setLineHeight(50);

  printer.println(F("\"Mlcanie"));
  printer.println(F("je jedna"));
  printer.println(F("z vlastnosti"));
  printer.println(F("dokonalosti.\""));

  printer.setLineHeight();
  printer.setSize('S');
  
  printer.println();
  printer.println(F("Franz Kafka|Das Schloß"));
  printer.print(F("("));

  printer.inverseOn();
  printer.print(F(" EAUDIO "));
  printer.inverseOff();
  
  printer.print(F(", "));
  
  printer.inverseOn();
  printer.print(F(" EBOOK "));
  printer.inverseOff();
  printer.println(F(")"));

  
  printer.justify('L');

  
  printer.println();
  printer.println();

  printer.println(F("Hladas viac knih, alebo by si"));
  printer.println(F("rad videl pribehy z kníh na"));
  printer.println(F("inych mediach? Chcel by si si"));
  printer.println(F("pozicat knihy kdekolvek? "));
  printer.println(F("Digitalna kniznica \"Onleihe\" "));
  printer.print(F("ti ponuka viac nez 14 000 medii od "));
//  inych mediach? Chcel by si si pozicat knihy kdekolvek? Digitalna kniznica \"Onleihe\" ti ponuka viac než 14 000 medii od "));
  printer.inverseOn();
  printer.print(F(" E-KNIH ")); 
  printer.inverseOff();
  printer.print(F(" cez ")); 
  printer.inverseOn();
  printer.print(F(" HUDBU "));
  printer.inverseOff();
  printer.print(F(", "));
  printer.inverseOn();
  printer.print(F(" AUDIO-KNIHY "));
  printer.inverseOff();
  printer.print(F(" az po "));
  printer.inverseOn();
  printer.print(F(" FILMY "));
  printer.inverseOff();
  printer.print(F(" pristupne  "));
  printer.println(F("v tvojom vrecku."));
  
  printer.println();
  printer.println();
  
  printer.println(F("Ako na to?"));
  printer.println(F("1.Zaregistruj si Onleihe ucet")); 
  printer.print(F(" " ));
  printer.println(F(" cez bit.ly/mygoethe"));

  printer.println(F("2.Stiahni si sikovnu mobilnu"));
  printer.println(F("  aplikaciu cez "));

  printer.println(F("    onelink.to/onleihe"));
  
  printer.println(F("3.Prihlas sa"));
  printer.println(F("4.Teraz mozes citat, pozerat"));
  printer.println(F("  a pocuvat na svojom telefone."));

  
  printer.println();

  printer.println(F("Onleihe funguje aj na tvojom po-citaci alebo tablete. Zaregi-   struj sa cez bit.ly/mygoethe    alebo sa prihlas na my.goethe.de"));

  printer.println();
  printer.println();
  printer.feed(2);

  printer.sleep();      // Tell printer to sleep
  delay(3000L);         // Sleep for 3 seconds
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
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
