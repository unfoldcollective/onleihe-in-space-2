#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "CubicEase.h"

// LedStrip Class wraps Adafruit_NeoPixel class to add custom animations
class LedStrip {
  private:
    Adafruit_NeoPixel leds;
    CubicEase ease;
    unsigned long previousMillisEase  = 0;
    unsigned long previousMillisFade  = 0;
    unsigned long previousMillisBlink = 0;
    int currentStepEase = 0;
    int currentStepFade = 0;
    float currentTEase  = 0;
    bool blinkBool;
  
  public:
    LedStrip (int ledPin, int ledCount) {
      leds = Adafruit_NeoPixel(ledCount, ledPin, NEO_GRB + NEO_KHZ800);
    }

    // Sets all LEDs to off, but DOES NOT update the display;
    // call leds.show() to actually turn them off after this.
    void clear()
    {
      for (int i=0; i<leds.numPixels(); i++)
      {
        leds.setPixelColor(i, 0);
      }
    }

    void begin () {
      leds.begin();
    }

    void show () {
      leds.show();
    }

    uint32_t Color(int r, int g, int b){
      return leds.Color(r, g, b);
    }

    void setPixelColor (int pixelIndex, uint32_t color) {
      leds.setPixelColor(pixelIndex, color);
    }

    /* set pixel color of range between int from and int to to Color c 
     * also works in when from > to
     */
    void colorRange(int from, int to, uint32_t c) {
      for (int i= min(from, to); i <= max(from,to); i++) {
        leds.setPixelColor(i, c);
      }
      leds.show();
    }

    /* alternate range between from and to between color1 and color2 every interval ms
     */
    void blinkRange (unsigned long currentMillis, int from, int to, int interval, uint32_t color1, uint32_t color2){
      if ((unsigned long)(currentMillis - previousMillisBlink) >= interval) {
        if (blinkBool) {
          colorRange(from, to, color1);
        }
        else {
          colorRange(from, to, color2);
        }
        blinkBool = !blinkBool;
        previousMillisBlink = currentMillis;
      }
    }

    /* easeIn from pixel until pixel (not index), over duration (ms), in color
     * using millis() to work asynchronously
     * return true when animation is ongoing
     * return false when animation has ended
     */
    bool easeInRangeMillis(unsigned long currentMillis, int from, int until, float duration, uint32_t color){
      int distance = until-from;
      int direction = (distance > 0) - (distance < 0); // calculate sign
      ease.setDuration(duration/1000);
      ease.setTotalChangeInPosition(abs(distance));
      
      double easedPosition;
      int rounded, diff, ledIndex;
      int nSteps = 100;
      int last = -1;

      float intervalEase = duration/nSteps;
      
      if ((unsigned long)(currentMillis - previousMillisEase) > intervalEase) {        
        easedPosition = ease.easeInOut(currentTEase);
        rounded = round(easedPosition);
        diff = rounded - last;
        
        // light up leds from last to rounded
        for (int j=1;j<=diff;j++) {
          ledIndex = from+(direction*last)+(direction*j);
          leds.setPixelColor(ledIndex, color);
        }
        leds.show();
        last = rounded;
        
        currentTEase += ((duration/1000) / nSteps);
        currentStepEase += 1;
        previousMillisEase = currentMillis;
        if (currentStepEase == nSteps) {
          currentTEase = 0;
          currentStepEase = 0;
          colorRange(from, until, Color(0,0,0));
          return false;
        }
        return true;
      }
      return true;
    }

    /* fade out range between from and to from brightness over duration
     * using millis() to work asynchronously
     */
    void fadeOutRangeFromBrightnessMillis(unsigned long currentMillis, int from, int to, int brightness, int duration){      
      int intervalFade = 40; // 40 ms == 1/25 second
      int nSteps = duration / intervalFade;
      float dBrightness = 1.0*brightness/nSteps;
      int newBrightness;
      if ((unsigned long)(currentMillis - previousMillisFade) == intervalFade) {
        // update the pixels
        currentStepFade++;
        newBrightness = round(brightness - (currentStepFade * dBrightness));
        for (int i=from; i <= to; i++) {
          leds.setPixelColor(i, leds.Color(newBrightness,newBrightness,newBrightness));
        }
        leds.show();
    
        if (currentStepFade == nSteps) {
          currentStepFade = 0;       
        }
        previousMillisFade = currentMillis;
      }
    }
};
